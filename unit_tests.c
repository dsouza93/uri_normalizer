#include <check.h> /* catch unit-test framework */
#include "normalize.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>

bool
normalize_uri_helper(const char *uri, const char *expected_normal){

    size_t uri_ct = strlen(uri);
    // The size of the normalized URL can be one character longer than the current url
    int buff_size = uri_ct + 2;
    fprintf(stderr, "BUFFER SIZE = %d\n", buff_size);
    int err;
    char uri_normal [buff_size];
    memset(uri_normal, 0, sizeof uri_normal);
    uri_ct = strlen(uri);

    err = normalize_uri(uri, uri_ct, uri_normal, buff_size);

    fprintf(stderr,"\n\n");

    if(err){
        return false;
    }

    if(expected_normal && strcmp(expected_normal, uri_normal) == 0){
        return true;
    }

    return false;
}

START_TEST(test_dot_segment)
{
  char path_buffer[1000];
  memset(path_buffer,0,1000);
  int err;
  fprintf(stderr,"******************************\n");
  fprintf(stderr,"*     DOT SEGMENT TESTS      *\n");
  fprintf(stderr,"******************************\n");

  fprintf(stderr,"../bar test\n");
  fprintf(stderr,"---------------------------------------------------------------------------------------------\n");
  err = remove_dot_segments("../bar", path_buffer, 1000); 
  fprintf(stderr,"New Path Size = %d\n", err);
  ck_assert_str_eq("bar", path_buffer);
  memset(path_buffer,0,1000);
  fprintf(stderr,"\n");

  fprintf(stderr,"./bar test\n");
  fprintf(stderr,"---------------------------------------------------------------------------------------------\n");
  err = remove_dot_segments("./bar", path_buffer, 1000); 
  fprintf(stderr,"New Path Size = %d\n", err);
  ck_assert_str_eq("bar", path_buffer);
  memset(path_buffer,0,1000);
  fprintf(stderr,"\n");
 
  fprintf(stderr,".././bar test\n");
  fprintf(stderr,"---------------------------------------------------------------------------------------------\n");
  err = remove_dot_segments(".././bar", path_buffer, 1000); 
  fprintf(stderr,"New Path Size = %d\n", err);
  ck_assert_str_eq("bar", path_buffer);
  memset(path_buffer,0,1000);
  fprintf(stderr,"\n");

  fprintf(stderr,"./../bar test\n");
  fprintf(stderr,"---------------------------------------------------------------------------------------------\n");
  err = remove_dot_segments("./../bar", path_buffer, 1000); 
  fprintf(stderr,"New Path Size = %d\n", err);
  ck_assert_str_eq("bar", path_buffer);
  memset(path_buffer,0,1000);
  fprintf(stderr,"\n");

  fprintf(stderr,"/foo/./bar test\n");
  fprintf(stderr,"---------------------------------------------------------------------------------------------\n");
  err = remove_dot_segments("/foo/./bar", path_buffer, 1000); 
  fprintf(stderr,"New Path Size = %d\n", err);
  ck_assert_str_eq("/foo/bar", path_buffer);
  memset(path_buffer,0,1000);
  fprintf(stderr,"\n");

  fprintf(stderr,"/bar/./ test\n");
  fprintf(stderr,"---------------------------------------------------------------------------------------------\n");
  err = remove_dot_segments("/bar/./", path_buffer, 1000); 
  ck_assert_str_eq("/bar/", path_buffer);
  fprintf(stderr,"New Path Size = %d\n", err);
  memset(path_buffer,0,1000);
  fprintf(stderr,"\n");

  fprintf(stderr,"/. test\n");
  fprintf(stderr,"---------------------------------------------------------------------------------------------\n");
  err = remove_dot_segments("/.", path_buffer, 1000); 
  fprintf(stderr,"New Path Size = %d\n", err);
  ck_assert_str_eq("/", path_buffer);
  memset(path_buffer,0,1000);
  fprintf(stderr,"\n");

  fprintf(stderr,"/bar/. test\n");
  fprintf(stderr,"---------------------------------------------------------------------------------------------\n");
  err = remove_dot_segments("/bar/.", path_buffer, 1000); 
  fprintf(stderr,"New Path Size = %d\n", err);
  ck_assert_str_eq("/bar/", path_buffer);
  memset(path_buffer,0,1000);
  fprintf(stderr,"\n");

  fprintf(stderr,"/foo/../bar test\n");
  fprintf(stderr,"---------------------------------------------------------------------------------------------\n");
  err = remove_dot_segments("/foo/../bar", path_buffer, 1000); 
  fprintf(stderr,"New Path Size = %d\n", err);
  ck_assert_str_eq("/bar", path_buffer);
  memset(path_buffer,0,1000);
  fprintf(stderr,"\n");

  fprintf(stderr,"/bar/../ test\n");
  fprintf(stderr,"---------------------------------------------------------------------------------------------\n");
  err = remove_dot_segments("/bar/../", path_buffer, 1000); 
  fprintf(stderr,"New Path Size = %d\n", err);
  ck_assert_str_eq("/", path_buffer);
  memset(path_buffer,0,1000);
  fprintf(stderr,"\n");

  fprintf(stderr,"/.. test\n");
  fprintf(stderr,"---------------------------------------------------------------------------------------------\n");
  err = remove_dot_segments("/..", path_buffer, 1000); 
  fprintf(stderr,"New Path Size = %d\n", err);
  ck_assert_str_eq("/", path_buffer);
  memset(path_buffer,0,1000);
  fprintf(stderr,"\n");
  
  fprintf(stderr,"/bar/.. test\n");
  fprintf(stderr,"---------------------------------------------------------------------------------------------\n");
  err = remove_dot_segments("/bar/..", path_buffer, 1000); 
  fprintf(stderr,"New Path Size = %d\n", err);
  ck_assert_str_eq("/", path_buffer);
  memset(path_buffer,0,1000);
  fprintf(stderr,"\n");

  fprintf(stderr,"/foo/bar/.. test\n");
  fprintf(stderr,"---------------------------------------------------------------------------------------------\n");
  err = remove_dot_segments("/foo/bar/..", path_buffer, 1000); 
  fprintf(stderr,"New Path Size = %d\n", err);
  ck_assert_str_eq("/foo/", path_buffer);
  memset(path_buffer,0,1000);
  fprintf(stderr,"\n");

  fprintf(stderr,"Single . test\n");
  fprintf(stderr,"---------------------------------------------------------------------------------------------\n");
  err = remove_dot_segments(".", path_buffer, 1000); 
  fprintf(stderr,"New Path Size = %d\n", err);
  ck_assert_str_eq("", path_buffer);
  memset(path_buffer,0,1000);
  fprintf(stderr,"\n");

  fprintf(stderr,"Single .. test\n");
  fprintf(stderr,"---------------------------------------------------------------------------------------------\n");
  err = remove_dot_segments("..", path_buffer, 1000); 
  fprintf(stderr,"New Path Size = %d\n", err);
  ck_assert_str_eq("", path_buffer);
  memset(path_buffer,0,1000);
  fprintf(stderr,"\n");

  fprintf(stderr,"Test foo/bar/.. test\n");
  fprintf(stderr,"---------------------------------------------------------------------------------------------\n");
  err = remove_dot_segments("foo/bar/..", path_buffer, 1000); 
  fprintf(stderr,"New Path Size = %d\n", err);
  ck_assert_str_eq("foo/", path_buffer);
  memset(path_buffer,0,1000);
  fprintf(stderr,"\n");

  fprintf(stderr,"Test Empty Path Segment\n");
  fprintf(stderr,"---------------------------------------------------------------------------------------------\n");
  err = remove_dot_segments("", path_buffer, 1000); 
  fprintf(stderr,"New Path Size = %d\n", err);
  ck_assert_str_eq("", path_buffer);
  memset(path_buffer,0,1000);
  fprintf(stderr,"\n");

}
END_TEST

START_TEST(test_normalize)
{
  fprintf(stderr,"******************************\n");
  fprintf(stderr,"*     Normalize TESTS        *\n");
  fprintf(stderr,"******************************\n");

  fprintf(stderr,"Testing passing too small of a URI to normalize\n");
  fprintf(stderr,"---------------------------------------------------------------------------------------------\n");
  ck_assert(!normalize_uri_helper("ht", NULL));

  fprintf(stderr,"Testing passing non http/https protocol\n");
  fprintf(stderr,"---------------------------------------------------------------------------------------------\n");
  ck_assert(!normalize_uri_helper("ht:", NULL));

  fprintf(stderr,"Passing a uri with half encoded value at end\n");
  fprintf(stderr,"---------------------------------------------------------------------------------------------\n");
  ck_assert(!normalize_uri_helper("http://www.google.co%4", NULL));

  fprintf(stderr,"Passing a uri with half encoded value in the middle\n");
  fprintf(stderr,"---------------------------------------------------------------------------------------------\n");
  ck_assert(!normalize_uri_helper("http://www.google.co%4psomethin/Path", NULL));

  fprintf(stderr,"Passing a uri with an empty path parameter\n");
  fprintf(stderr,"---------------------------------------------------------------------------------------------\n");
  ck_assert(normalize_uri_helper("http://www.google.com", "http://www.google.com/"));

  fprintf(stderr,"Passing a uri with an empty path parameter and additional query params\n");
  fprintf(stderr,"---------------------------------------------------------------------------------------------\n");
  ck_assert(normalize_uri_helper("http://www.google.com?query1=foo&query2=bar", "http://www.google.com/?query1=foo&query2=bar"));

  fprintf(stderr,"Empty path parameter with port\n");
  fprintf(stderr,"---------------------------------------------------------------------------------------------\n");
  ck_assert(normalize_uri_helper("http://www.google.com:9301?query1=foo&query2=bar", "http://www.google.com:9301/?query1=foo&query2=bar"));

  fprintf(stderr,"Passing a uri with a username and password\n");
  fprintf(stderr,"---------------------------------------------------------------------------------------------\n");
  ck_assert(normalize_uri_helper("http://dylan%40:PaSsword@www.Goo%47le.coM:80/", "http://dylan%40:PaSsword@www.google.com/"));

  fprintf(stderr,"Testing Removal of standard http Port\n");
  fprintf(stderr,"---------------------------------------------------------------------------------------------\n");
  ck_assert(normalize_uri_helper("http://kelloggsTester.com:80/Something/Here", "http://kelloggstester.com/Something/Here"));

  fprintf(stderr,"Testing Removal of standard https Port\n");
  fprintf(stderr,"---------------------------------------------------------------------------------------------\n");
  ck_assert(normalize_uri_helper("https://kelloggsTester.com:443/Something/Here", "https://kelloggstester.com/Something/Here"));

  fprintf(stderr,"Testing passing of non-standard http Port\n");
  fprintf(stderr,"---------------------------------------------------------------------------------------------\n");
  ck_assert(normalize_uri_helper("http://kelloggsTester.com:443/Something/Here", "http://kelloggstester.com:443/Something/Here"));

  fprintf(stderr,"Testing passing of non-standard https Port\n");
  fprintf(stderr,"---------------------------------------------------------------------------------------------\n");
  ck_assert(normalize_uri_helper("https://kelloggsTester.com:80/Something/Here", "https://kelloggstester.com:80/Something/Here"));

  fprintf(stderr,"Testing the standard removal of . and .. in the path \n");
  fprintf(stderr,"---------------------------------------------------------------------------------------------\n");
  ck_assert(normalize_uri_helper("https://kelloggsTester.com:80/Something/Here/././bigson/../me", "https://kelloggstester.com:80/Something/Here/me"));

  fprintf(stderr,"Testing . and .. segments in non path components\n");
  fprintf(stderr,"---------------------------------------------------------------------------------------------\n");
  ck_assert(normalize_uri_helper("https://kelloggsTester.com:80/Something/Here?query1=/././bigson/../me", "https://kelloggstester.com:80/Something/Here?query1=/././bigson/../me"));
  
  fprintf(stderr,"Testing standard decdoing of multiple characters\n");
  fprintf(stderr,"---------------------------------------------------------------------------------------------\n");
  ck_assert(normalize_uri_helper("https://kelloggs%54ester.com/%53omething/Here", "https://kelloggstester.com/Something/Here"));

  fprintf(stderr,"Testing passing encoded reserved characters\n");
  fprintf(stderr,"---------------------------------------------------------------------------------------------\n");
  ck_assert(normalize_uri_helper("https://kelloggs%54ester.com/%53omething/Here%3f", "https://kelloggstester.com/Something/Here%3F"));

  fprintf(stderr,"Testing empty hostname with userinfon\n");
  fprintf(stderr,"---------------------------------------------------------------------------------------------\n");
  ck_assert(!normalize_uri_helper("https://dylan:something@", NULL));

  fprintf(stderr,"Testing empty uri after http://\n");
  fprintf(stderr,"---------------------------------------------------------------------------------------------\n");
  ck_assert(!normalize_uri_helper("http://", NULL));

  //We just don't want the following things to seg fault 
  fprintf(stderr,"Testing http:///////\n");
  fprintf(stderr,"---------------------------------------------------------------------------------------------\n");
  ck_assert(!normalize_uri_helper("http:///////", NULL));

  fprintf(stderr,"Testing empty uri after http://?/\n");
  fprintf(stderr,"---------------------------------------------------------------------------------------------\n");
  ck_assert(!normalize_uri_helper("http://?/", NULL));

}
END_TEST

Suite *
jwt_suite(void)
{
  Suite *s;
  TCase *tc_core;

  s = suite_create("JWT");

  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_normalize);
  tcase_add_test(tc_core, test_dot_segment);

  suite_add_tcase(s, tc_core);

  return s;
}

int
main(void)
{
  int number_failed;

  Suite *s;
  SRunner *sr;

  s  = jwt_suite();
  sr = srunner_create(s);

  srunner_run_all(sr, CK_VERBOSE);
  number_failed = srunner_ntests_failed(sr);

  srunner_free(sr);

  return (number_failed == 0) ? 0 : 1;
}

# uri_normalizer
This normalize_uri function is used put http/https URLs in their normal form. The function takes a URL as a string and a buffer to write to. 

Unit tests are provided. In order to run these, install the check framework and compile with:

gcc -Wall unit_tests.c normalize.c -o unit_tests -lcheck

# Resources
Function normalize_uri is used to normalize URIs for in the implementation of the cdniuc claim in URI Signing - https://tools.ietf.org/html/draft-ietf-cdni-uri-signing-16
Noramlization Specs - https://tools.ietf.org/html/rfc7230#section-2.7.3
                    - https://tools.ietf.org/html/rfc3986#section-6.2.2

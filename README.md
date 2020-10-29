# Kupyna

## Description
- *main.c* - provides a command line user interface;
- *kupyna.h, kupyna.c* - contains the main part of Kalyna alghorithm, fully compliant to standard: https://eprint.iacr.org/2015/885.pdf ;
- *static.h, static.c* - contains the static part of algorithm (e.g. *s_box, kupyna_config ...*);
- *types.h* - contains typedef aliases;
- *util.h, util.c* - contains utility methods (e.g. *shift, mul_bytes ...*);

### Unit test:
- *test_kupyna* - contains unit test for kupyna standard

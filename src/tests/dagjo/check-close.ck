# -*- perl -*-
use strict;
use warnings;
use tests::tests;
check_expected (IGNORE_USER_FAULTS => 1, [<<'EOF']);
(check-close) begin
no-close: exit(-1)
(check-close) end
check-close: exit(0)
EOF
pass;

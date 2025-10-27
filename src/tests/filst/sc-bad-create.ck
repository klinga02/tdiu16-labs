# -*- perl -*-
use strict;
use warnings;
use tests::tests;
check_expected ([<<'EOF']);
(sc-bad-create) begin
(sc-bad-create) Creating a 32 KiB file
(sc-bad-create) end
sc-bad-create: exit(0)
EOF
pass;

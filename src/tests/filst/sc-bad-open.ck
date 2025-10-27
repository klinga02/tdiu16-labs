# -*- perl -*-
use strict;
use warnings;
use tests::tests;
check_expected ([<<'EOF', <<'EOF']);
(sc-bad-open) begin
sc-bad-open: exit(-1)
EOF
(sc-bad-open) begin
(sc-bad-open) end
sc-bad-open: exit(0)
EOF
pass;

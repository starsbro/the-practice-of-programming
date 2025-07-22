# markov.pl: markov chain algorithm for 2-word prefixes

$MAXGEN = 10000;
$NONWORD = "\n";
$w1 = $w2 = $NONWORD;                 # initial state
while (<>) {                          # read each line of input
    foreach (split) {
        push(@{$statetab{$w1}{$w2}}, $_);
        ($w1, $w2) = ($w2, $_);       # multiple assignment
    }
}
push(@{$statetab{$w1}{$w2}}, $NONWORD);     # add tail, the heart of the program is this line
$w1 = $w2 = $NONWORD;
for ($i = 0; $i < $MAXGEN; $i++) {
    $suf = $statetab{$w1}{$w2};             # array reference
    last unless defined $suf && @$suf;     # exit if no suffixes
    $r = int(rand @$suf);                   # @$suf is number of elems
    last if ($t = $suf->[$r]) eq $NONWORD; # exit if we hit NONWORD
    print "$t\n";
    ($w1, $w2) = ($w2, $t);                # advance chain
}

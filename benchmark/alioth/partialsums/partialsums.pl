# The Computer Language Shootout
# http://shootout.alioth.debian.org/
# Contributed by Emanuele Zeppieri

my $N = shift || 2_500_000;

my ($s0, $s1, $s2, $s3, $s4, $s5, $s6, $s7, $s8, $k2, $k3, $ksin, $kcos);
my $alt = -1;
sub TWO_THIRDS () { 2/3 }

for (1..$N) {
    $s0 += TWO_THIRDS ** ($_ - 1);
    $s1 += 1 / sqrt;
    $s2 += 1 / ( $_ * ($_ + 1));
    $s3 += 1 / ( ($k3 = ($k2 = $_ * $_) * $_) * ($ksin = sin) * $ksin );
    $s4 += 1 / ( $k3 * ($kcos = cos) * $kcos );
    $s5 += 1 / $_;
    $s6 += 1 / $k2;
    $s7 += ($alt = -$alt) / $_;
    $s8 += $alt / (2 * $_ - 1)
}

printf "%0.9f\t(2/3)^k\n"             , $s0;
printf "%0.9f\tk^-0.5\n"              , $s1;
printf "%0.9f\t1/k(k+1)\n"            , $s2;
printf "%0.9f\tFlint Hills\n"         , $s3;
printf "%0.9f\tCookson Hills\n"       , $s4;
printf "%0.9f\tHarmonic\n"            , $s5;
printf "%0.9f\tRiemann Zeta\n"        , $s6;
printf "%0.9f\tAlternating Harmonic\n", $s7;
printf "%0.9f\tGregory\n"             , $s8

# The Computer Language Shootout
# http://shootout.alioth.debian.org/
# implemented by Greg Buchholz
# streamlined by Kalev Soikonen

sub ITER ()	{ 50 }
sub LIMITSQR ()	{ 2.0 ** 2 }

my ($w, $h, $i);
my ($Cr, $Ci, $Zr, $Zi, $Tr, $Ti);

sub dot {
    $Cr = 2 * $_[0] / $w - 1.5;
    $Ci = 2 * $_[1] / $h - 1.0;

    $Zr = $Zi = $Tr = $Ti = 0.0;
    $i = ITER;
    (
	$Zi = 2.0 * $Zr * $Zi + $Ci,
	$Zr = $Tr - $Ti + $Cr,
	$Ti = $Zi * $Zi,
	$Tr = $Zr * $Zr
    ) until ($Tr + $Ti > LIMITSQR || !$i--);
    return ($i == -1);
}

$w = $h = shift || 80;
print "P4\n$w $h\n";
for my $y (0..$h-1) {
    print pack 'B*', pack 'C*', map dot($_, $y), 0..$w-1;
}


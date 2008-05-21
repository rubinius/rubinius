# The Computer Language Shootout
# http://shootout.alioth.debian.org/
#
# Contributed by Markus Peter

sub A {
    my ( $i, $j ) = @_;
    return 1.0 / ( ( $i + $j ) * ( $i + $j + 1 ) / 2 + $i + 1 );
}

sub Av {
    my ( $u ) = @_;
    my $len= $#$u;
    my @v= ();
    for my $i ( 0..$len ) {
        $v[$i] = 0;
        for my $j ( 0..$len ) { $v[$i] += A( $i, $j ) * $u->[$j]; }
    }
    return \@v;
}

sub Atv {
    my ( $u ) = @_;
    my $len= $#$u;
    my @v= ();
    for my $i ( 0..$len ) {
        $v[$i] = 0;
        for my $j ( 0..$len ) { $v[$i] += A( $j, $i ) * $u->[$j]; }
    }
    return \@v;
}

sub AtAv {
    my ( $u ) = @_;
    return Atv(Av($u));
}

my $i;
my $N = @ARGV ? $ARGV[0] : 500;

my $v;
my $u= [];
for $i (0..$N-1) { $u->[$i]=1; }
for $i (0..9) {
    $v= AtAv( $u);
    $u= AtAv( $v);
}

$vBv = $vv = 0;
for $i (0..$N-1) {
    $vBv += $u->[$i] * $v->[$i];
    $vv += $v->[$i] * $v->[$i];
}
printf( "%0.9f\n", sqrt( $vBv / $vv ) );

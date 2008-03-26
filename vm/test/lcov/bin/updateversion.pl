#!/usr/bin/perl -w
#
# Update version and date strings in LCOV files
#
# Usage: updateversion.pl <lcov directory> <version> <date> <release>
#

use strict;

sub find_version($);
sub find_date($);
sub replace($);

our $directory = $ARGV[0];
our $version = $ARGV[1];
our $date = $ARGV[2];
our $release = $ARGV[3];

our @filelist = ("bin/gendesc", "bin/genhtml", "bin/geninfo",  "bin/genpng",
		 "bin/lcov", "man/gendesc.1", "man/genhtml.1",
		 "man/geninfo.1", "man/genpng.1" ,"man/lcov.1", "README",
		 "rpm/lcov.spec" );

our @version_pattern = ('(LTP GCOV extension version )(\d+\.\d+)',
			'(\.TH \w+ \d+ \"\w+ )(\d+\.\d+)',
			'(Version:\s*)(\d+\.\d+)');

our @date_pattern = ('(\.TH \w+ \d+ \"\w+ \d+.\d+" )(\d+-\d+-\d+)',
		     '(Last\s+changes:\s+)(\d+-\d+-\d+)');

our @release_pattern = ('(Release:\s*)(\d+)');

our $file;

if (!($directory && $version && $date))
{
	print(STDERR "usage: updateversion.pl DIRECTORY VERSION DATE ".
	      "RELEASE\n");
	exit(1);
}

foreach $file (@filelist)
{
	replace("$directory/$file");
	find_version("$directory/$file");
	find_date("$directory/$file");
}

exit(0);



sub find_version($)
{
	my $filename = $_[0];
	my $line;
	my $pattern;
	local *HANDLE;

	open(HANDLE, "<$filename") or die("ERROR: cannot open $filename!\n");

	while ($line = <HANDLE>)
	{
		foreach $pattern (@version_pattern)
		{
			if ($line =~ /$pattern/)
			{
				print("$filename: version is $2\n");
			}
		}
	}

	close(HANDLE);
}

sub find_date($)
{
	my $filename = $_[0];
	my $line;
	my $pattern;
	local *HANDLE;

	open(HANDLE, "<$filename") or die("ERROR: cannot open $filename!\n");

	while ($line = <HANDLE>)
	{
		foreach $pattern (@date_pattern)
		{
			if ($line =~ /$pattern/)
			{
				print("$filename: date is $2\n");
			}
		}
	}

	close(HANDLE);
}


sub replace($)
{
	my $filename = $_[0];
	my $line;
	my $pattern;
	local *HANDLE;
	local *OUT;

	open(HANDLE, "<$filename") or die("ERROR: cannot open $filename!\n");
	open(OUT, ">$filename.out")
		or die("ERROR: cannot open $filename.out for writing!\n");

	while ($line = <HANDLE>)
	{
		foreach $pattern (@version_pattern)
		{
			$line =~ s/$pattern/$1$version/g;
		}

		foreach $pattern (@date_pattern)
		{
			$line =~ s/$pattern/$1$date/g;
		}

		foreach $pattern (@release_pattern)
		{
			$line =~ s/$pattern/$1$release/g;
		}

		print(OUT $line);
	}

	close(OUT);
	close(HANDLE);

	system("chmod $filename.out --reference $filename");
	system("mv $filename.out $filename");
}

use strict;
use warnings;

my $HeaderDir = "TLib";
my @SubDirs = qw(tool);
my $BuildDir = "Build";
my @Platforms = qw(win32 x64);
my @Configures = qw(Release Debug);
my $PackDir = "Pack";
my $IncludeDir = "include";
my $LibDir = "lib";
my $module_name = "TLib";

my @Targets = ("TLibTool.lib");

# clean pack dir
if( -d $PackDir )
{
	system("rmdir /s/q $PackDir");
}

# craete dir
system("mkdir $PackDir");
system("mkdir $PackDir\\$IncludeDir");
system("mkdir $PackDir\\$LibDir");

# copy header files
print "Copy header files\n";
my $Pwd = "$PackDir\\$IncludeDir\\$HeaderDir";
system("mkdir $Pwd");
system("copy /Y $HeaderDir\\*.h $Pwd");
foreach my $subdir (@SubDirs)
{
	system("mkdir $Pwd\\$subdir");
	system("copy /Y $HeaderDir\\$subdir\\*.h $Pwd\\$subdir");
	system("copy /Y $HeaderDir\\$subdir\\*.proto $Pwd\\$subdir");
}

# copy library
foreach my $platform (@Platforms)
{
	my $Pwd = "$PackDir\\$LibDir\\$platform\\$module_name";
	system("mkdir $Pwd");
    foreach my $configure (@Configures)
	{
		my @fielnames = split(' ', `dir /W $BuildDir\\$platform\\$configure`);
		foreach my $file (@fielnames)
		{
			foreach my $target (@Targets)
			{
				next if $file =~ /\[.*\]/; # skip if a dir
				if( $file =~ /$target/)
				{
					my $tgtfile = $file;
					if( $configure eq "Debug" )
					{
						$tgtfile =~ s/\./_d\./g;
					}
					
					print "Copy $BuildDir\\$platform\\$configure\\$file to $Pwd\\$tgtfile\n";
					system("copy /Y $BuildDir\\$platform\\$configure\\$file $Pwd\\$tgtfile");
					last;
				}
			}
		}
	}
}

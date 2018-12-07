#!/usr/bin/perl
use strict;
use warnings;
#
# backtick executes a command and collects stdout 
# stderr is not collected but goes to output


# for example, if we want to see only the errors and
# warnings we can ignore the stdout.  To see stdout:
# print "the info is: $info\n";
# print "the status is $?\n";

# @ denotes an array
my @alltests;
for (my $i = 1; $i <= 14; $i++) {
    push(@alltests, "test".$i);
}

push(@alltests, "required1");
push(@alltests, "required2");

my $failed = 0;
foreach my $testname (@alltests) {
    print "running $testname\n";
    my $outname = "out_".$testname;
    my $stuname = "stu_".$outname;
    my $info = `./lab2 < ../tests/$testname > $stuname`;
    if ($? != 0) {
        print "$testname did not run correctly\n";
        $failed = 1;
    } else {
        $info = `diff -w $stuname ../tests/$outname`;
        if ($? != 0) {
            print "$testname ran but differences\n";
            $failed = 1;
        }
    }
}
if ($failed == 0) {
    print "no failures found\n";
}

#print "the info is: $info\n";
#print "the status is $?\n";

# system executes a command but only gets final return value
# all output just goes to stdout

    #
    # $make = "cd $user; make clean; make; cd ..";
    # $rc = 0xffff & system $make;
    # if ($rc != 0) {
    #     die "system $make failed: $?";
    # }


#    a more elaborate way to test return value
#    $rc = 0xffff & system $make;
#    if ($rc == 0) {
#        print "ran with normal exit\n";
#    }
#    elsif ($rc == 0xff00) {
#        print "command failed: $!\n";
#    }
#    elsif (($rc & 0xff) == 0) {
#        $rc >>= 8;
#        print "ran with non-zero exit status $rc\n";
#    }
#    else {
#        print "ran with ";
#        if ($rc & 0x80) {
#            $rc &= ~0x80;
#            print "coredump from ";
#        }
#        print "signal $rc\n";
#    }

# reg expression  (upper case negates the following)
#   \d [0-9]
#   \w [a-zA-Z0-9_]
#   \s [ \r\t\n\f]
#   * zero or more
#   + one or more
#   ? zero or one

# File tests
#     -x file or directory is executable
#     -e file or directory exists
#     -z file exists and has zero size
#     -s non zero size
#     -f a file
#     -d a directory
#

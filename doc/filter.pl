#!/usr/bin/perl -w

my @state;
my (%shape, %incoming);
my $end = 2;
while (<>) {
    if (/digraph/ || /\}/) {
	# add these wrapper lines back later
    } elsif (/(\S+) \-> (\S+)/) {
	my ($src, $dest) = ($1, $2);
	if (/\%/ || /EOF/ || /START/ || /NULL/) {

	    if (/\%4/) { s/\[/\[style=bold;dir=both;arrowtail=odot;color=black;/; $node{$src} = 'shape=rect' }
	    elsif (/\%3/) { s/\[/\[style=solid;dir=both;arrowtail=odot;color=darkslategrey;/; $node{$src} = 'shape=triangle'  }
	    elsif (/\%2/) { s/\[/\[style=dashed;dir=both;arrowtail=odot;color=darkslategrey;/; $node{$src} = 'shape=doublecircle'  }
	    else { s/\[/\[style=dotted;color=darkslategrey;/; $node{$src} = 'style=dotted' unless defined $node{$src}  }

	    if (/\/"/) {
		s/\[/\[arrowhead=empty;/;
	    }
	    
	    s/"/\$/g;
	    s/\%/_/g;
	    s/([ACGT])/\\mbox\{$1\}/;
	    s/EOF/\\epsilon/g;
	    s/START/\\epsilon/g;
	    s/NULL/\\epsilon/g;

	    s/label=\$.*\$//;  # just remove the label altogether, too cluttered

	    push @trans, $_;
	    ++$incoming{$dest};
	}
    } elsif (/(\S+) \[/) {
	my $s = $1;
	unless (/Code/ || /Control/) {
	    s/\[/\[style=dotted;/;
	}
	if (/"End/) {
	    s/"End.*"/"E"/;
	} elsif (/"Start/) {
	    s/"Start.*"/"S"/;
	    $incoming{$s} = 1;  # hack to keep start...
	} else {
	    #	    s/label="\S+#(\d+) (\S+)/label="$2$1/;
	    s/label="\S+#(\d+) (\S+)/label="$2/;
	    s/ +"/"/;
	    s/\*//g;
	}
	push @state, $_;
	push @stateId, $s;
    }
}

print "digraph G {\nrankdir=LR;\n", @trans;
for my $i (0..$#state) {
    my $s = $stateId[$i];
    if ($incoming{$s}) {
	if (defined $node{$s}) {
	    $state[$i] =~ s/\[/\[$node{$s};/;
	}
	print $state[$i];
    }
}
print "}\n";

#!/usr/bin/env perl
use Mojolicious::Lite;
use File::Slurp qw(slurp);
use utf8;

sub handle_request {
	my $self  = shift;
	my $type  = $self->stash('source') // q{};
	my $state = $self->stash('state') // q{};

	given ("$type/$state") {
		when ('all/on')       { system('pgctl_on') }
		when ('all/off')      { system('pgctl_off') }
		when ('all/fadeup')   { system('pgctl_fadeup') }
		when ('all/fadedown') { system('pgctl_fadedown') }
		when ('all/wait')     { system('pgctl_wait') }
		when ('mains/on')     { system('pgctl mains_on') }
		when ('mains/off')    { system('pgctl mains_off') }
		when ('light/on')     { system('pgctl light_on') }
		when ('light/off')    { system('pgctl light_off') }
		when ('light/60p')    { system('pgctl light_60p') }
		when ('light/40p')    { system('pgctl light_40p') }
		when ('light/20p')    { system('pgctl light_20p') }
		when ('light/10p')    { system('pgctl light_10p') }
		when ('light/strobe') { system('pgctl light_strobe') }
		when (/s([1-4])\/p([1-5])_(on|off)/) {
			system("pgctl s${1}_p${2}_${3}");
		}
	}

	$self->render( 'main', title => 'pgctl', );
}

helper show_link => sub {
	my ( $self, $type, $state, $label ) = @_;

	my $state_is = slurp( "/tmp/.pgctl_${type}", { err_mode => 'quiet', } )
	  // q{};
	$label //= $state;
	$label =~ s{p$}{%};


	return sprintf(
		'<a class="%s %s"  href="/%s/%s">%s%s</a>',
		( $state =~ m{^\d} ) ? "l$state" : $state,
		( $state eq $state_is ) ? 'cur' : q{},
		$type, $state, $label,
		($state eq $state_is) ? '<br/>✓' : q{},
	);
};

app->config(
	hypnotoad => {
		accepts  => 10,
		listen   => ['http://*:8093'],
		pid_file => '/tmp/pgctl.pid',
		workers  => 1,
	},
);

get '/'               => \&handle_request;
get '/:source/:state' => \&handle_request;

app->start();

__DATA__

@@ main.html.ep
<!DOCTYPE html>
<html>
<head>
	<title><%= $title %></title>
	<meta charset="utf-8">
	<style type="text/css">

	body {
		font-family: Sans-Serif;
		font-size: 300%;
		width: 200%;
	}

	div.break {
		clear: both;
	}

	div.all,
	div.mains,
	div.light,
	div.s1,
	div.s2,
	div.s3,
	div.s4 {
		float: left;
		text-align: center;
		margin-left: 1em;
		font-variant: small-caps;
	}

	div.desc {
		width: 4em;
		height: 2em;
		text-align: center;
		border-bottom: 1px dashed black;
	}

	div.all a,
	div.mains a,
	div.light a,
	div.s1 a,
	div.s2 a,
	div.s3 a,
	div.s4 a {
		display: block;
		width: 4em;
		height: 3em;
		text-align: center;
		text-decoration: none;
	}

	a.on {
		color: black;
		background-color: white;
	}

	a.off {
		color: white;
		background-color: black;
	}

	a.l10p {
		color: white;
		background-color: #222222;
	}

	a.l20p {
		color: white;
		background-color: #333333;
	}

	a.l40p {
		color: white;
		background-color: #666666;
	}

	a.l60p {
		color: black;
		background-color: #bbbbbb;
	}

	a.strobe {
		color: black;
		background-color: yellow;
	}

	a.cur {
		font-weight: bold;
	}

	</style>
</head>
<body>
<div class="outer">
<div class="light">
<div class="desc">light</div>
% for my $state (qw(on off 10p 20p 40p 60p strobe)) {
%== show_link('light', $state)
% }
</div>
<div class="mains">
<div class="desc">mains</div>
%== show_link('mains', 'on')
%== show_link('mains', 'off')
</div>
<div class="all">
<div class="desc">all</div>
%== show_link('all', 'on')
%== show_link('all', 'off')
%== show_link('all', 'fadeup', '~&uarr;')
%== show_link('all', 'fadedown', '~&darr;')
%== show_link('all', 'wait')
</div>
<div class="s1">
<div class="desc">switch 1</div>
%== show_link('s1', 'p1_on')
%== show_link('s1', 'p1_off');
%== show_link('s1', 'p2_on')
%== show_link('s1', 'p2_off');
%== show_link('s1', 'p3_on')
%== show_link('s1', 'p3_off');
%== show_link('s1', 'p4_on')
%== show_link('s1', 'p4_off');
%== show_link('s1', 'p5_on')
%== show_link('s1', 'p5_off');
</div>
<div class="s2">
<div class="desc">switch 2</div>
%== show_link('s2', 'p1_on')
%== show_link('s2', 'p1_off');
%== show_link('s2', 'p2_on')
%== show_link('s2', 'p2_off');
%== show_link('s2', 'p3_on')
%== show_link('s2', 'p3_off');
%== show_link('s2', 'p4_on')
%== show_link('s2', 'p4_off');
%== show_link('s2', 'p5_on')
%== show_link('s2', 'p5_off');
</div>
<div class="s3">
<div class="desc">switch 3</div>
%== show_link('s3', 'p1_on')
%== show_link('s3', 'p1_off');
%== show_link('s3', 'p2_on')
%== show_link('s3', 'p2_off');
</div>
<div class="s4">
<div class="desc">switch 4</div>
%== show_link('s4', 'p1_on')
%== show_link('s4', 'p1_off');
%== show_link('s4', 'p2_on')
%== show_link('s4', 'p2_off');
%== show_link('s4', 'p3_on')
%== show_link('s4', 'p3_off');
%== show_link('s4', 'p4_on')
%== show_link('s4', 'p4_off');
%== show_link('s4', 'p5_on')
%== show_link('s4', 'p5_off');
</div>
</div>
</body>
</html>

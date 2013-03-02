#!/usr/bin/env perl
use strict;
use warnings;
use utf8;

use Mojolicious::Lite;
use File::Slurp qw(slurp);

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

	say "qwop";
	$self->render( 'main', title => 'pgctl', );
}

helper show_link => sub {
	my ( $self, $type, $state, $label ) = @_;

	my $state_is = slurp( "/tmp/.pgctl_${type}", { err_mode => 'quiet', } )
	  // q{};
	$label //= $state;
	$label =~ s{p$}{%};

	my $snippet = <<'EOF';
<script type="text/javascript">
$("#l%s%s").click(function() {
	$.get("/%s/%s")
})
</script>
EOF

	return sprintf(
		'<div class="href %s %s" id="l%s%s">%s%s</div>' . $snippet,
		( $state =~ m{^\d} ) ? "l$state" : $state,
		( $state eq $state_is ) ? 'cur' : q{},
		$type,
		$state,
		$label,
		( $state eq $state_is ) ? '<br/>✓' : q{},
		$type,
		$state,
		$type,
		$state,
	);
};

app->config(
	hypnotoad => {
		accept_interval => 0.2,
		listen          => ['http://*:8093'],
		pid_file        => '/tmp/pgctl.pid',
		workers         => 1,
	},
);

get '/'               => \&handle_request;
get '/:source/:state' => \&handle_request;

app->start();

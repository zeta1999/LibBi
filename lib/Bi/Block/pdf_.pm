=head1 NAME

pdf_ - block for univariate pdf actions.

=cut

package Bi::Block::pdf_;

use parent 'Bi::Block';
use warnings;
use strict;

our $BLOCK_ARGS = [];

sub validate {
    my $self = shift;
    
    $self->process_args($BLOCK_ARGS);
    
    if (@{$self->get_blocks} > 0) {
        die("a 'pdf_' block may not contain nested blocks\n");
    }
}

1;

=head1 AUTHOR

Lawrence Murray <lawrence.murray@csiro.au>


=head1 NAME

Bi::Model::DimAlias - dimension alias.

=head1 SYNOPSIS

    use Bi::Model::DimAlias;

=head1 INHERITS

L<Bi::Expression>

=head1 METHODS

=over 4

=cut

package Bi::Model::DimAlias;

use parent 'Bi::Node';
use warnings;
use strict;

use Carp::Assert;

=item B<new>(I<name>, I<range>)

Constructor.

=over 4

=item I<name>

Alias of the dimension, as string.

=item I<range>

Range of the alias, as L<Bi::Expression::Range> object.

=back

Returns the new object.

=cut
sub new {
    my $class = shift;
    my $name = shift;
    my $range = shift;
    
    assert (!defined $range || $range->isa('Bi::Expression::Range')) if DEBUG;

    if (defined $range && !$range->is_const) {
        die("a dimension range on the left must be a constant expression.\n");
    }
        
    my $self = {
        _name => $name,
        _range => $range
    };
    bless $self, $class;

    return $self;
}

=item B<clone>

Return a clone of the object.

=cut
sub clone {
    my $self = shift;
    
    my $clone = {
        _name => $self->get_name,
        _range => $self->get_range->clone
    };
    bless $clone, ref($self);
    
    return $clone; 
}

=item B<get_name>

Get the name of the alias.

=cut
sub get_name {
    my $self = shift;
    return $self->{_name};
}

=item B<has_name>

Is there a name?

=cut
sub has_name {
    my $self = shift;
    return defined $self->{_name};
}

=item B<get_range>

Get the range of the alias, as a L<Bi::Expression::Range> object.

=cut
sub get_range {
    my $self = shift;
    return $self->{_range};
}

=item B<set_range>(I<range>)

Set the range of the alias.

=cut
sub set_range {
    my $self = shift;
    my $range = shift;

    assert (!defined $range || $range->isa('Bi::Expression::Range')) if DEBUG;
    
    if (!$range->is_const) {
        die("a dimension range on the left must be a constant expression.\n");
    }
    
    $self->{_range} = $range;
}

=item B<has_range>

Is there a range?

=cut
sub has_range {
	my $self = shift;
	return defined $self->{_range};
}

=item B<get_size>

Get the size.

=cut
sub get_size {
    my $self = shift;
    
    return $self->get_range->get_end->eval_const - $self->get_range->get_start->eval_const + 1;
}

=item B<gen_index>

Generate an index for this alias, as L<Bi::Expression::Index> object.

=cut
sub gen_index {
    my $self = shift;
    
    return new Bi::Expression::Index(new Bi::Expression::DimAliasIdentifier($self));
}

=item B<gen_range>

Generate a range for this alias, as L<Bi::Expression::Range> object.

=cut
sub gen_range {
    my $self = shift;
    
    return $self->get_range->clone;
}

=item B<accept>(I<visitor>, ...)

Accept visitor.

=cut
sub accept {
    my $self = shift;
    my $visitor = shift;
    my @args = @_;
    
    $self = $visitor->visit_before($self, @args);
    return $visitor->visit_after($self, @args);
}

=item B<equals>(I<obj>)

Does object equal I<obj>?

=cut
sub equals {
    my $self = shift;
    my $obj = shift;
    
    return ref($obj) eq ref($self) && $self->get_name eq $obj->get_name;
}

1;

=back

=head1 SEE ALSO

L<Bi::Expression>

=head1 AUTHOR

Lawrence Murray <lawrence.murray@csiro.au>

=head1 VERSION

$Rev: 2921 $ $Date: 2012-08-12 13:49:45 +0800 (Sun, 12 Aug 2012) $

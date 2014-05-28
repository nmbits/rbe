
$: << File.expand_path('..', File.dirname(__FILE__))
require 'rbe'

p1 = B::Point.new(1.0, 2.0)
p1.set 20.5, 30.55
p1.print_to_stream

p2 = B::Point.new(3.0, 4.0)
r = B::Rect.new(p2, p1)
r.print_to_stream

r.left = 100.0
r.print_to_stream


c = RBe::Cpp::Class["Size"]
if c
  # TODO
  %w(Width Height SetWidth SetHeight).each do |s|
    c.remove_functions s
  end

  # c.ctors.first.set_option(:custom, true)

end

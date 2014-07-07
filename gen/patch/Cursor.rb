
c = RBe::Cpp::Class["Cursor"]

if c
  c.super_class = nil
  c.ctors.each{|f| f.set_option :custom, true }
end

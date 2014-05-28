
c = RBe::Cpp::Class["Rect"]
if c
  # TODO
  %w(OffsetToSelf OffsetBySelf InsetBySelf).each do |s|
    c.functions(s).each{|f| f.set_option :noimp, true }
  end

  # c.ctors.first.set_option(:custom, true)

end

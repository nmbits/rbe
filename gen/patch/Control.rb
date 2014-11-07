
c = RBe::Cpp::Class["Control"]
if c
  c.wrapper = true
  c.mixin = "Invoker"

  # noimp
  %w(
    SetIcon
    SetIconBitmap
    IconBitmap
    Perform
  ).each do |name|
    c.functions(name).each do |f|
      f.set_option :noimp, true
    end
  end

  # GetPreferredSize
  f = c.functions("GetPreferredSize")[0]
  f.args.each{|a| a.out = true}
end

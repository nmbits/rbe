
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

  # custom hook
  %w(
    GetPreferredSize
  ).each do |name|
    c.functions(name).each do |f|
      f.set_option :custom_hook, true
    end
  end

  # custom
  %w(
    GetPreferredSize
  ).each do |name|
    c.functions(name).each do |f|
      f.set_option :custom, true
    end
  end
end

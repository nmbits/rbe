
c = RBe::Cpp::Class["Application"]
if c
  c.wrapper = true

  %w(QuitRequested
     Pulse
     ReadyToRun
     MessageReceived
     ArgvReceived
     AppActivated
     RefsReceived
     AboutRequested
     DispatchMessage).each do |s|
    c.set_hook s
  end

  c.functions("DispatchMessage").each do |f|
    f.set_option :custom_hook, true
  end

  c.functions("ArgvReceived").each do |f|
    f.set_option :custom_hook, true
    f.set_option :custom, true
  end

  %w(Run
     Quit
     QuitRequested).each do |name|
    c.functions(name).each do |f|
      f.set_option :assert_locked, true
    end
  end

  c.functions("QuitRequested").each{|f| f.ret_default = "false" }

  %w(Run
     Quit).each do |s|
    c.functions(s).each{|f| f.set_option :custom, true }
  end

  %w(SetCursor
     GetAppInfo
     WindowAt
     Perform).each do |s|
    c.functions(s).each{|f| f.set_option :noimp, true }
  end

  c.ctors.first.set_option(:custom, true)

  c.add_inner_variable RBe::Cpp::Variable.new("bool", "fDispatchMessageCalled", "false")
  c.add_inner_variable RBe::Cpp::Variable.new("bool", "fRunCalled", "false")
end

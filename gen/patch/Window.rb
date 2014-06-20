
c = ::RBe::Cpp::Class["Window"]
if c
  c.wrapper = true

  c.ctors.each{|f| f.set_option :custom, true }

  # hooks (TODO: Zoom())
  %w(FrameMoved         FrameResized
     MenusBeginning     MenusEnded
     MessageReceived    DispatchMessage  QuitRequested
     ScreenChanged
     WindowActivated
     WorkspaceActivated WorkspaceChanged).each do |name|
    c.set_hook name
  end

  # custom hooks
  %w(DispatchMessage).each do |name|
    c.functions(name).each do |f|
      f.set_option :custom_hook, true
    end
  end

  c.functions("QuitRequested").each do |f| f.ret_default = "true" end

  # prelock
  # note: SendBehind may need to be :custom
  %w(AddChild RemoveChild CountChildren ChildAt
     Minimize  
     SendBehind
     Flush
     Sync
     DisableUpdates       EnableUpdates
     BeginViewTransaction EndViewTransaction InViewTransaction
     IsFront
     SetSizeLimits UpdateSizeLimits
     SetDecoratorSettings GetDecoratorSettings
     NeedsUpdate
     IsMinimized
     SetTitle
     AddToSubset RemoveFromSubset
     SetType SetLook SetFeel SetFlags
     SetWindowAlignment GetWindowAlignment
     Workspaces SetWorkspaces
     MoveBy MoveTo
     ResizeBy ResizeTo
     CenterIn CenterOnScreen
     Hide).each do |name|
    c.functions(name).each{|f| f.set_option :prelock, true }
  end

  # custom (TODO)
  %w(Show Run).each do |name|
    c.functions(name).each{|f| f.set_option :custom, true }
  end

  # noimp (temporal)
  %w(GetSizeLimits
     AddShortcut
     DefaultButton SetDefaultButton
     UpdateIfNeeded
     CenterOnScreen
     GetWindowAlignment).each do |name|
    c.functions(name).each{|f| f.set_option :noimp, true }
  end

  # noimp (temporal; related to BView)
  %w(ChildAt
     FindView CurrentFocus LastMouseMovedView).each do |name|
    c.functions(name).each{|f| f.set_option :noimp, true }
  end

  # only for View, not LayoutItem
  %w(AddChild RemoveChild).each do |name|
    c.functions(name).each{|f| f.set_option :custom, true }
  end

  # noimp (temporal; ConvertTo, ConvertFrom)
  %w(ConvertToScreen ConvertFromScreen).each do |name|
    c.functions(name).each{|f| f.set_option :noimp, true }
  end

  # noimp (temporal; Menus)
  %w(SetKeyMenuBar KeyMenuBar).each do |name|
    c.functions(name).each{|f| f.set_option :noimp, true }
  end

  # noimp (temporal; Layout)
  %w(SetLayout GetLayout).each do |name|
    c.functions(name).each{|f| f.set_option :noimp, true }
  end
  
  # noimp
  %w(Perform).each do |name|
    c.functions(name).each{|f| f.set_option :noimp, true }
  end
end

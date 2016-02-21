
c = ::RBe::Cpp::Class["View"]
if c
  c.wrapper = true

  c.ctors.each{|f| f.set_option :custom, true }

  # hooks
  %w(AttachedToWindow   AllAttached
     DetachedFromWindow AllDetached
     Draw DrawAfterChildren
     FrameMoved FrameResized
     GetPreferredSize ResizeToPreferred
     KeyDown KeyUp
     MessageReceived
     MouseDown MouseMoved MouseUp Pulse
     TargetedByScrollView
     WindowActivated
     LayoutChanged
     ).each{|name| c.set_hook name}

  # GetPreferredSize
  f = c.functions("GetPreferredSize").first
  f.args.each{|a| a.out = true}

  # custom_hook

  %w(KeyDown KeyUp
     TargetedByScrollView).each do |name|
    c.functions(name).each{|f| f.set_option :custom_hook, true }
  end

  # custom
  %w(KeyDown KeyUp
     ConvertToParent ConvertFromParent
     ConvertToScreen ConvertFromScreen
     TargetedByScrollView
     Invalidate).each do |name|
    c.functions(name).each{|f| f.set_option :custom, true }
  end

  # check_lock
  %w(Bounds Frame
     Flags SetFlags
     Hide Show
     IsFocus
     LeftTop
     SetResizingMode
     ScrollTo
     SetEventMask SetMouseEventMask
     SetScale Scale
     SetLineMode LineJoinMode LineCapMode LineMiterLimit
     SetDrawingMode DrawingMode
     SetBlendingMode
     MovePenTo MovePenBy PenLocation SetPenSize PenSize
     SetHighColor HighColor
     SetLowColor LowColor
     SetViewColor ViewColor
     ForceFontAliasing
     DrawChar
     StrokeEllipse
     StrokeArc
     StrokeRect
     StrokeRoundRect
     StrokeTriangle
     StrokeLine
     BeginLineArray AddLine EndLineArray
     CopyBits
     InvertRect
     CountChildren
     RemoveSelf
     MoveBy MoveTo
     ResizeBy ResizeTo).each do |name|
    c.functions(name).each{|f| f.set_option :check_lock, true }
  end

  # check_owner_lock
  %w(Sync
     BeginRectTracking EndRectTracking
     PushState PopState
     SetOrigin Origin).each do |name|
    c.functions(name).each{|f| f.set_option :check_owner_lock, true }
  end

  # noimp (TODO; temporal)
  %w(SetViewCursor
     DragMessage
     GetMouse ScrollBar
     GetBlendingMode
     SetFont GetFont GetFontHeight
     TruncateString
     ClipToPicture ClipToInversePicture
     GetClippingRegion ConstrainClippingRegion
     DrawBitmapAsync DrawBitmap
     DrawString
     FillEllipse
     FillArc
     StrokeBezier FillBezier
     StrokePolygon FillPolygon
     FillRect
     FillRoundRect
     FillRegion
     FillTriangle
     StrokeShape FillShape ClipToShape ClipToInverseShape
     SetDiskMode
     BeginPicture AppendToPicture EndPicture
     SetViewBitmap ClearViewBitmap
     SetViewOverlay ClearViewOverlay
     DrawPicture DrawPictureAsync
     AddChild RemoveChild
     Perform
     LayoutAlignment ExplicitAlignment SetExplicitAlignment
     GetHeightForWidth
     SetLayout GetLayout
     LayoutContext LayoutAlignment
     SetToolTip ToolTip ShowToolTip GetToolTipAt
     ScrollWithMouseWheelDelta
     StringWidth GetStringWidths
     SetTransform Transform
     LayoutInvalidated DoLayout
     SetViewUIColor ViewUIColor HighUIColor SetHighUIColor LowUIColor SetLowUIColor).each do |name|
    c.functions(name).each{|f| f.set_option :noimp, true }
  end
end

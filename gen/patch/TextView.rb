
c = ::RBe::Cpp::Class["TextView"]
if c
  c.wrapper = true

  # GetPreferredSize
  f = c.functions("GetPreferredSize").first
  f.args.each{|a| a.out = true}

  f = c.functions("GetSelection").first
  f.args.each{|a| a.out = true}

  f = c.functions("GetInsets").first
  f.args.each{|a| a.out = true}

  %w(Perform
     SetText Insert GetText Cut Copy Paste AcceptsPaste
     SetFontAndColor GetFontAndColor
     SetRunArray RunArray
     PointAt
     FindWord
     GetTextRegion
     SetAlignment Alignment
     SetColorSpace ColorSpace
     Undo UndoState
     GetDragParameters
     GetHeightForWidth
     InsertText
  ).each do |name|
    c.functions(name).each{|f| f.set_option :noimp, true}
  end

  # MouseMoved
  f = c.functions("MouseMoved").first
  f.args[2].accept_nil = true

  c.ctors.each{|f| f.set_option :custom, true}
end

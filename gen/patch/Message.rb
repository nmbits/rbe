
c = RBe::Cpp::Class["Message"]
if c
  # TODO
  %w(GetInfo DropPoint AddString Append ReplaceString HasSameData).each do |s|
    c.functions(s).each{|f| f.set_option :noimp, true }
  end

  %w(SendReply Flatten Unflatten GetCurrentSpecifier AddAlignment AddPointer AddRef AddFlat AddData FindData FindAlignment ReplaceAlignment FindPointer ReplacePointer ReplaceRef FindRef FindFlat  ReplaceFlat ReplaceData HasFlat GetPointer SetPointer GetAlignment GetRect SetAlignment SetData).each do |s|
    c.functions(s).each{|f| f.set_option :noimp, true }
  end

  # TODO
  %w(FindRect FindPoint FindSize FindString FindInt8 FindUInt8 FindInt16 FindUInt16 FindInt32 FindUInt32 FindInt64 FindUInt64 FindBool FindFloat FindDouble FindPointer FindMessenger FindMessage AddStrings FindStrings AddString FindString ReplaceString SetString).each do |s|
    c.functions(s).each{|f| f.set_option :noimp, true }
  end

  # TODO
  %w(GetRect GetPoint GetSize SetRect SetPoint SetSize).each do |s|
    c.functions(s).each{|f| f.set_option :noimp, true }
  end


end

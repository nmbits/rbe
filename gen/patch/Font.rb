
c = RBe::Cpp::Class["Font"]
if c
  # TODO
  %w(SetFamilyAndStyle SetFamilyAndFace GetFamilyAndStyle Direction Blocks FileFormat
     GetTunedInfo TruncateString GetTruncatedStrings StringWidth GetStringWidths
     GetEscapements GetEdges GetHeight
     GetBoundingBoxesAsGlyphs GetBoundingBoxesAsString
     GetBoundingBoxesForStrings
     GetGlyphShapes GetHasGlyphs).each do |s|
    c.functions(s).each{|f| f.set_option :noimp, true }
  end

  c.ctors.first.set_option :custom, true

end

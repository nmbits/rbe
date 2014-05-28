
RBe::Cpp::Class.delete_class "Unarchiver"

c = RBe::Cpp::Class["Archivable"]
if c
  %w(Perform).each do |name|
    c.functions(name).each do |f|
      f.set_option :noimp, true
    end
  end
end

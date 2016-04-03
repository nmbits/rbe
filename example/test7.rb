
$: << File.expand_path('..', File.dirname(__FILE__))
require 'rbe'

B.DEBUG = true

class MyWindow < B::Window
  def quit_requested
    p :quit_requested
    B.app.post_message(B::QUIT_REQUESTED)
    true
  end
end

class MyApp < B::Application
  def create_window
    rect = B::Rect.new(20.0, 20.0, 300.0, 300.0)
    @window = MyWindow.new(rect, "Test", B::DOCUMENT_WINDOW, 0)
    menu = B::Menu.new "test"
    20.times do |i|
      if i % 5 == 4
        menu.add_separator_item
        item = nil
      elsif i % 3 == 2
        item = B::SeparatorItem.new
      else
        item = B::MenuItem.new("item #{i}", B::Message.new(i+10))
      end
      unless item.nil?
        menu.add_item item
        item.set_marked true if i % 2 == 0
      end
    end
	submenu = B::Menu.new "submenu"
	10.times do |i|
      item = B::MenuItem.new("item #{i}", B::Message.new(i+10))
      submenu.add_item item
	end
	menu.add_item submenu

    @window.add_child menu
    @window.show
  end

  def message_received(m)
    m.print_to_stream
    super
  end
  
  def ready_to_run
    set_pulse_rate 1000000
    set_name "This is Ruby"
    create_window
  end
  
  def quit_requested
    true
  end

  def pulse
    # GC.start
  end
end

# GC.disable
app = MyApp.new 'application/x-vnd.HaikuTest'
app.run

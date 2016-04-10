
$: << File.expand_path('..', File.dirname(__FILE__))
require 'rbe'

B.DEBUG = false
DEBUG = true

class MyWindow < B::Window
  def quit_requested
    p :quit_requested
    B.app.post_message(B::QUIT_REQUESTED)
    true
  end

  def message_received(message)
    case message.what
    when 0x1010
      type = message.get_u_int32("type", 0)
      label = message.get_string("label")
      buttons = message.get_u_int32("buttons", 0)
      puts "type = #{type}, buttons = #{buttons}"
      button_texts = [nil, nil, nil]
      buttons.times do |i|
        button_texts[i] = "button #{i+1}"
      end
      alert = B::Alert.new "alert test", "Alert window: #{label}", *button_texts, B::WIDTH_AS_USUAL, type
      alert.go
    when 0x1011
      self.post_message B::QUIT_REQUESTED
    end
  rescue => e
    p e
    exit
  end
end

class MyApp < B::Application
  def create_window
    rect = B::Rect.new(20.0, 20.0, 300.0, 300.0)
    @window = MyWindow.new(rect, "Test", B::DOCUMENT_WINDOW, 0)
    menu_bar = B::MenuBar.new B::Rect.new(0, 0, 300, 40), "test"
    menu = B::Menu.new "File"
    [["empty", B::EMPTY_ALERT],
     ["info", B::INFO_ALERT],
     ["idea", B::IDEA_ALERT],
     ["warning", B::WARNING_ALERT],
     ["stop", B::STOP_ALERT]].each do |text, type|
      submenu = B::Menu.new text
      3.times do |i|
        message = B::Message.new 0x1010
        message.add_u_int32("type", type)
        message.add_string("label", text)
        message.add_u_int32("buttons", i+1);
        submenu.add_item B::MenuItem.new("buttons = #{i+1}", message)
      end
      menu.add_item submenu
    end
    menu.add_separator_item
    menu.add_item B::MenuItem.new("Quit", B::Message.new(0x1011))
    menu_bar.add_item menu
    @window.add_child menu_bar
    @window.show
  end

  def ready_to_run
    # set_pulse_rate 1000000
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

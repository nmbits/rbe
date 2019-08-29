
$: << File.expand_path('..', File.dirname(__FILE__))
require 'rbe'

B.DEBUG = true
DEBUG = true

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
    menu_bar = B::MenuBar.new B::Rect.new(0, 0, 300, 40), "test"
    menu = B::Menu.new "File"
    menu.add_item B::MenuItem.new("GC.start", B::Message.new(0x1012))
    menu.add_item B::MenuItem.new("Quit", B::Message.new(0x1011))
    menu_bar.add_item menu
    @window.add_child menu_bar
    tab_view = B::TabView.new B::Rect.new(0, 50, 300, 90), "tabview"
    @window.add_child tab_view

    2.times do |i|
      view = B::TextView.new B::Rect.new(0, 100, 300, 150), "test", B::Rect.new(0, 100, 300, 150), B::FOLLOW_ALL
      tab_view.add_tab view
    end

    def tab_view.mouse_moved(*a)
      super
    rescue => e
      p e
      p a
      exit
    end

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

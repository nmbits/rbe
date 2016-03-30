
$: << File.expand_path('..', File.dirname(__FILE__))
require 'rbe'

B.DEBUG = true

class MyWindow < B::Window
  def quit_requested
    B.app.post_message(B::QUIT_REQUESTED)
    true
  end

  def message_received(message)
    message.print_to_stream
  rescue => e
    p e
  ensure
    super
  end
end

class MyApp < B::Application
  def create_window
    rect = B::Rect.new(20.0, 20.0, 300.0, 300.0)
    @window = MyWindow.new(rect, "Test", B::DOCUMENT_WINDOW, 0)
    message = B::Message.new 0xf0f0
    button = B::Button.new B::Rect.new(0.0, 0.0, 200.0, 200.0), "test", "test", message

    def button.message_received(*a)
      p :message_received
      super
    end

    @window.add_child button
    @window.show
  end
  
  def ready_to_run
    # set_pulse_rate 1000000
    set_name "This is Ruby"
    create_window
  end
  
#  def quit_requested
#    true
#  end

  def pulse
    GC.start
  end
end

# GC.disable
app = MyApp.new 'application/x-vnd.HaikuTest'
app.run

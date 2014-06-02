
$: << File.expand_path('..', File.dirname(__FILE__))

require 'rbe.so'

B.DEBUG = true

class TestWindow < B::Window
  def dispatch_message(message, handler)
    p :dispatch_message
    super
  end

  def frame_moved(point)
    p :frame_moved
    point.print_to_stream
  end
  
  def window_activated(val)
    p :window_activated
    puts val.to_s
  end

  def quit_requested
    p :quit_requested
    true
  end
end

class TestApp < B::Application
  def ready_to_run
    r = B::Rect.new 20, 20, 300, 300
    @window = TestWindow.new(r, "test", B::DOCUMENT_WINDOW_LOOK, B::NORMAL_WINDOW_FEEL, 0, B::CURRENT_WORKSPACE)
    @window.show
    p "aaaa"
#    set_pulse_rate 5000000
  end

#  def pulse
#    quit
#  end
end

a = TestApp.new "application/x-vnd.Test"
a.run

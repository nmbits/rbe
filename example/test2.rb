
$: << File.expand_path('..', File.dirname(__FILE__))
require 'rbe'

class MyHandler < B::Handler
end

class MyLooper < B::Looper

  def dispatch_message(m, h)
    p "MyLooper#dispatch_message"
    super
  end

  def message_received(m)
    p "MyLooper#message_received"
  end

  def quit_requested
    p "MyLooper#quit_requested"
    true
  end

end

class MyApp < B::Application
  def create_looper
    @looper = MyLooper.new
    @looper_thread = @looper.run
    @looper.post_message(9876543, @looper)
  end
  
  def post_to_looper
    @looper.post_message(9876543, @looper)
  end
  
  def quit_looper
    p :quit_looper
    @looper.lock
    p :looper_lock
    @looper.quit
    p :looper_quit
  end

  def ready_to_run
    p :ready_to_run
    set_pulse_rate 1000000
    create_looper
  end

  def quit_requested
    p :quit_requested
    true
  end

  def pulse
    p :pulse
    @count ||= 0
    @count += 1
    post_to_looper
    if @count > 2
      @looper.post_message B::QUIT_REQUESTED
      @looper_thread.join
      quit
    end
  end

  def message_received(message)
    p :message_received
    p message.what
  end
end

B.DEBUG = true
app = MyApp.new 'application/x-vnd.HaikuTest'
p app.run

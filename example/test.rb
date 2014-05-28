
$: << File.expand_path('..', File.dirname(__FILE__))
require 'rbe'

class MyApp < B::Application
  def ready_to_run
    p :ready_to_run
    set_pulse_rate 1000000
    p looper
    set_name "ruby_process"
    p name
    post_message 987654321
    p handler_at(0)
    p handler_at(1)
  end
  
  def app_activated(active)
    p :app_activated
    super
  end

  def quit_requested
    p :quit_requested
    true
  end

  def pulse
    p :pluse
    @count ||= 0
    @count += 1
    quit if @count > 2
  end

  def dispatch_message(msg, handler)
    p :dispatch_message
    p handler
    super
  end

  def message_received(msg)
    p :message_received
    p msg.what
  end
end

B.DEBUG = true
app = MyApp.new 'application/x-vnd.HaikuTest'
p app.__id__
p app.run

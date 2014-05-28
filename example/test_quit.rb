
$: << File.join(File.dirname(__FILE__), "..")
require 'rbe.so'

B.DEBUG = true
DEBUG = true
Thread.DEBUG = 1


l = B::Looper.new
def l.message_received(m)
  m.print_to_stream
end

l.run
l.post_message(100);


#sleep 1

t = Thread.new(l) do |looper|
  10.times do
    looper.post_message(100)
    sleep 0.5
    GC.start
  end
end

# p B.app
# p l.locking_thread

t.join
l.lock
l.quit

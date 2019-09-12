$: << File.expand_path('..', File.dirname(__FILE__))
require 'rbe'

B.DEBUG = true
DEBUG = true

class MyWindow < B::Window
  def quit_requested
    B.app.post_message(B::QUIT_REQUESTED)
    true
  end
end

class MyView < B::View
  def initialize(name, flag)
    super
    @points = []
  end

  def mouse_moved(point, flag, message)
    @points << point
    @points.shift if @points.length > 20
    invalidate
  end

  def draw(rect)
    return if @points.empty?
    move_pen_to @points.first
    (@points.size - 1).times do |i|
      stroke_line @points[i + 1]
    end
  end
end

class MyApp < B::Application
  def create_window
    rect = B::Rect.new(20.0, 20.0, 300.0, 300.0)
    window = MyWindow.new(rect, "Test", B::DOCUMENT_WINDOW, 0)
    # view = MyView.new B::Rect.new(0.0, 0.0, 200.0, 200.0), "test", B::FOLLOW_ALL_SIDES, B::WILL_DRAW
    view = MyView.new "test", B::WILL_DRAW
    layout = B::GroupLayout.new(B::VERTICAL)
    window.set_layout layout
    item = layout.add_view view
    item.set_explict_alignment(B::Alignment.new(B::ALIGN_HORIZONTAL_CENTER, B::ALIGN_VERTICAL_CENTER))
    window.show
  end
  
  def ready_to_run
    set_name "This is Ruby"
    create_window
  end
  
  def quit_requested
    true
  end
end

# GC.disable
app = MyApp.new 'application/x-vnd.HaikuTest'
app.run

var ZOOM_VAL = 40;
function draw_circ(context, color, rad){
    context.beginPath();
    context.arc(rad,rad,rad,0,2*Math.PI);
    context.fillStyle = color;
    context.fill();
}
function draw_gradient(context, color, rad){
    var gradient = context.createRadialGradient(rad, rad, rad, rad, rad, 0);
    gradient.addColorStop(0, "rgba(0,0,0,0)");
    gradient.addColorStop(1, color);
    context.fillStyle = gradient;
    context.fillRect(0, 0, rad*2, rad*2);
}
function make_canv_of_size(width, height){
    var m_canvas = document.createElement('canvas');
    m_canvas.width = width;
    m_canvas.height = height;
    return m_canvas;
}
function make_circle_sprite(rad,color){
    var u_canvas = make_canv_of_size(2*rad,2*rad)
    var u_context = u_canvas.getContext("2d");
    draw_circ(u_context,color,rad);
    return u_canvas;
}
function make_gradient_sprite(rad,color){
    var u_canvas = make_canv_of_size(2*rad,2*rad)
    var u_context = u_canvas.getContext("2d");
    draw_gradient(u_context,color,rad);
    return u_canvas;
}
function make_food_sprite(){
    return make_gradient_sprite(ZOOM_VAL*FOOD_RADIUS,"green");
}
function make_unit_sprite(){
    return make_circle_sprite(ZOOM_VAL*COMBAT_BASE_RANGE/2,"red");
}
function draw_sprite(cen, context, sprite){
    var right_corner_x = Math.round(cen.x - sprite.width/2)
    var right_corner_y = Math.round(cen.y - sprite.height/2)
    context.drawImage(sprite, right_corner_x, right_corner_y)
}
function draw_game(map, context, canvas){
    var unit_sprite = make_unit_sprite()
    var food_sprite = make_food_sprite()
    context.clearRect(0, 0, canvas.width, canvas.height);
    map.get_item_locations(ZOOM_VAL).forEach(function(loc_item){
        if(loc_item.item.type == "_unit"){
            draw_sprite(loc_item.loc, context, unit_sprite)
        }
        else{
            draw_sprite(loc_item.loc, context, food_sprite)
        }
    })
}
function update_many_times(game_state){
    var update_number = document.getElementById("update_ammount").value;
    for(var i = 0; i < update_number; i++){
        set_random_actions(game_state)
        update_game(game_state)
    }
}

window.onload = function(){
    var global_canvas = document.getElementById("myCanvas");
    var global_context = global_canvas.getContext("2d");
    global_canvas.style = "scale: 50%;";
    var global_game = initialize_game()
    var should_update = true;
    function render(){
        if(should_update){
            draw_game(global_game.map, global_context, global_canvas)
            should_update = false
        }
        window.requestAnimationFrame(render)
    }
    render()
    document.getElementById("update_btn").onclick = function(){
        update_many_times(global_game)
        should_update = true;
    }
    var timeout_id = null;
    document.getElementById("start_running").onclick = function(){
        if(timeout_id == null){
            timeout_id = window.setInterval(function(){
                update_many_times(global_game)
                should_update = true;
            }, 10)
        }
    }
    document.getElementById("stop_running").onclick = function(){
        if(timeout_id != null){
            window.clearInterval(timeout_id)
            timeout_id = null
        }
    }
}

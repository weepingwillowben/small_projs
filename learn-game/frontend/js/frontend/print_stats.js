function new_text_node(node_ty, text){
    var header = document.createElement(node_ty);
    header.innerText = text
    return header;
}
function make_new_print_div(unique_id, values){
    var parent = document.createElement("div");
    parent.id = unique_id
    parent.appendChild(new_text_node('h3', unique_id))

    //Assign different attributes to the element.
    for (key in values){
        var val = values[key]
        var text = "key: " + key + ", value: " + val
        parent.appendChild(new_text_node('p', text))
    }

    return parent;
}
function remove_all_children_from_div(myNode){
    myNode.innerHTML = '';
}
function draw_all_items(items,map,element_id){
    var units_div = document.getElementById(element_id);
    remove_all_children_from_div(units_div)
    //Append the element in page (in span).
    items.forEach(function(unit){
        units_div.appendChild(make_new_print_div(element_id+unit._unique_id,unit));
    })
}
function rand_id_actions(units){
    return units.map(function(unit){
        var act = random_action()
        act._unique_id = unit._unique_id
        return act;
    })
}
function draw_stats(game_state){
    draw_all_items(game_state.units,game_state.map,"unit_printouts")
    draw_all_items(game_state.food_sources,game_state.map,"food_printouts")
    draw_all_items(rand_id_actions(game_state.units),game_state.map,"action_printouts")
}
window.onload = function(){
    var global_game = initialize_game()
    draw_stats(global_game)
    document.getElementById("update_btn").onclick = function(){
        var update_ammount = document.getElementById("update_ammount").value;
        for(var i = 0; i < update_ammount; i++){
            set_random_actions(global_game)
            update_game(global_game)
        }
        draw_stats(global_game)
    }
}

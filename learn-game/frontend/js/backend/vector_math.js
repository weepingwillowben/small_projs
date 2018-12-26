function square(x){
    return x * x;
}
function distance(loc1, loc2){
    return Math.sqrt(square(loc1.x - loc2.x) + square(loc1.y - loc2.y))
}
function vec_scalar_mult(vec,scalar){
    return {
        x: vec.x * scalar,
        y: vec.y * scalar,
    }
}
function subtract(p1, p2){
    return {
        x: p1.x - p2.x,
        y: p1.y - p2.y,
    }
}
function add(p1, p2){
    return {
        x: p1.x + p2.x,
        y: p1.y + p2.y,
    }
}
function angle_from_to(p_source,p_dest){
    var res_p = subtract(p_dest, p_source)
    return Math.atan2(res_p.y, res_p.x)
}
function euclidian_location(center, distance, angle){
    return {
        x: center.x + Math.cos(angle) * distance,
        y: center.y + Math.sin(angle) * distance,
    }
}
function wrap_coord(min_val,max_val,val){
    var size = max_val - min_val
    var wrap_val = val
    while(wrap_val < min_val){
        wrap_val = wrap_val + size
    }
    while(wrap_val > max_val){
        wrap_val = wrap_val - size
    }
    return wrap_val
}
function wrap_loc(wrap_width,wrap_height,loc){
    return {
        x: wrap_coord(0,wrap_width,loc.x),
        y: wrap_coord(0,wrap_height,loc.y),
    }
}
function mag_vector(vec){
    return Math.sqrt(square(vec.x) + square(vec.y))
}
function dist_to_path(start, end, cmp_loc){
    //this function gets the minimum distance of cmp_loc to any point along the path
    //currently an estimate, not exact
    var CHECK_POINTS = 10;
    var path_vec = subtract(end,start);
    var add_check = vec_scalar_mult(path_vec,mag_vector(path_vec)/CHECK_POINTS)

    var cur_point = start;
    var min_dist = 10e50;
    for(var i = 0; i <= CHECK_POINTS; i++){
        min_dist = Math.min(min_dist, distance(cur_point,cmp_loc))
        cur_point = add(cur_point, add_check);
    }
    return min_dist
}
function normify_angle(unnormed_angle){
    var two_pi = (2*Math.PI)
    return ((unnormed_angle % two_pi) + two_pi) % two_pi
}

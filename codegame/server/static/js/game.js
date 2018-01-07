

var boardel;
var game_name;
var spotsize = 30;

function over_all_board(xsize,ysize,bdata,fn){
    for(y = 0; y < ysize; y++){
        for(x = 0; x < xsize; x++){
            bdata[y][x].forEach(function(filename){
                fn(filename,x,y)
            })
        }
    }
}
function size_game_canvas(xsize,ysize){
    var canv =  document.getElementById('boardcanv');
    canv.width = xsize*spotsize;
    canv.height = ysize*spotsize;
}
function load_all_imgs(fnames,finalfn){
    var img_dict = {}
    var count = 0
    fnames.forEach(function(fname){
        var img = new Image();
        img.src = fname;
        img.addEventListener("load", function() {
            img_dict[fname] = img
            count += 1
            if(count >= fnames.length){
                //instead of returning a value, the value is used here
                finalfn(img_dict)
            }
        }, false);
    })
}
function get_fnames(xsize,ysize,bdata){
    var fnames = {}
    over_all_board(xsize,ysize,bdata,function(filename){
        fnames[filename] = ""
    })
    return Object.keys(fnames)
}
function draw_images(xsize,ysize,bdata,img_dict){
    var canvas = document.getElementById('boardcanv');
    var ctx = canvas.getContext('2d');
    over_all_board(xsize,ysize,bdata,function(fname,x,y){
        ctx.drawImage(img_dict[fname],x*spotsize,y*spotsize)
    })
}
function load_board_charachteristics(xsize,ysize,bdata){
    fnames = get_fnames(xsize,ysize,bdata)
    load_all_imgs(fnames,function(img_dict){
        draw_images(xsize,ysize,bdata,img_dict)
    })
}
function load_game_data(data){
    size_game_canvas(data.boardsize_x,data.boardsize_y)
    load_board_charachteristics(data.boardsize_x,data.boardsize_y,data.board)
}









function show(element){
    element.style.display = 'block'
}
function hide(element){
    element.style.display = 'none'
}
function new_game_popup(){
    show(document.getElementById("newgamepopup"))
}
function start_new_game(){
    $.post({
        url:"/start_new_game",
        success:load_game_data,
        dataType:"json"
    })
    hide(document.getElementById("newgamepopup"))
}
function save_game_popup(){

}
function load_game(game_name){
    $.post({
        url:"/load_game",
        data:game_name,
        success:load_game_data,
        dataType:"json"
    })
}
function create_game_name_button(name){
    newcell = document.createElement("BUTTON")
    newcell.addEventListener("click",function(){load_game(name)}) = "boardsquare"
    name_list = document.getElementById("game_name_list")
    name_list.appendChild(newcell)
}
function show_game_names(names){
    hide(document.getElementById("game_names_loading"))
    names.forEach(create_game_name_button)
}
function load_game_popup(){
    show(document.getElementById("newgamepopup"))
    show(document.getElementById("game_names_loading"))
    $.get({
        url:"/game_names",
        success:show_game_names,
        dataType:"json"
    })
}
window.onload = function(){
    size_game_canvas(50,20);
}

function logIn() {
    $(".signUp").remove();
    $(".logIn").remove();

    t = "\
        <div class = 'logIn'><br>\
            <input class = 'input' type = 'text' value = 'your id' name = 'user_id' id = 'user_id' onclick = \"if(value == defaultValue) {value = ''; this.style.color = '#000';}\"></input>\
            <br>\
            <input class = 'input' type = 'text' value = 'password' name = 'password' id = 'password' onclick = \"if(value == defaultValue) {value = ''; type = 'password'; this.style.color = '#000';}\"></input>\
            <br><br>\
            <button class='button submit' onmousedown='submit()'>Submit</button>\
        </div>";
    $(".MainBox").append(t);
}

function signUp() {
    $(".signUp").remove();
    $(".logIn").remove();

    t = "\
        <div class = 'signUp'><br>\
            <input class = 'input' type = 'text' value = 'your name' name = 'username' id = 'username' onclick = \"if(value == defaultValue) {value = ''; this.style.color = '#000';}\"></input>\
            <br>\
            <input class = 'input' type = 'text' value = 'password' name = 'password' id = 'password' onclick = \"if(value == defaultValue) {value = ''; type = 'password'; this.style.color = '#000';}\"></input>\
            <br><br>\
            <button class='button submit' onmousedown='submit()'>Submit</button>\
        </div>";
    $(".MainBox").append(t);
}

function submit() {
    var $user_id = $("#user_id");
    var $password = $("#password");

    if ($user_id.val() == "" || 
        $user_id.val() == "user_id" ||
        $password.val() == "" ||
        $password.val() == "password") {
        return;
    }
    else {
        $.ajax({
            url : "http://192.168.222.128:9999/room.html",
            type : "get",
            data : {
                action  : "login",
                user_id: $user_id.val(),
                password: $password.val()
            },
            success:function(result) {
                if (result == "Wrong ID or password!") {
                    alert(result);
                } else {
                    var newPage = window.open("about:blank", "_blank");
                    newPage.document.write(result);
                }
            }
        })
    }


}
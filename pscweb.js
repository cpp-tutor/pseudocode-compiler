// pscweb.js : support library for Pseudocode Compiler web interface

var input_index = 0, pseudocode_examples;

function readline() {
    let input = document.getElementById('user_input').value;
    var input_read = '';
    var more = input.indexOf(',', input_index);
    if (more != -1) {
         let begin = input_index;
         input_index = more + 1;
         input_read = input.substring(begin, more);
    }
    else {
         input_read = input.substring(input_index);
    }
    if (input_read != '') {
        print(input_read);
    }
    else {
        print('#Empty input#');
    }
    return input_read;
}

function print(line) {
    let max_lines = 26, max_length = 60;
    let lines = document.getElementById('program_output').innerHTML.split('\n').length + line.length / max_length;
    if (lines > max_lines) {
        alert('Too many lines of output! Please clear the output window.');
        throw new Error('Window full');
    }
    while(line.length > max_length) {
        document.getElementById('program_output').innerHTML += line.substring(0, max_length) + '\n';
        line = line.substring(max_length);
    }
    document.getElementById('program_output').innerHTML += line + '\n';
}

function clear_output() {
    document.getElementById('program_output').innerHTML = '';
}

function clear_input() {
    document.getElementById('input').elements['program_source'].value = '';
}

function toutf8(str) {
    var urifmt = '' + encodeURIComponent(str), x = function(s) {
        return String.fromCharCode(parseInt(s.substring(1), 16));
    }
    return urifmt.replace(/%../g, x);
}

function base64enc(input) {
    let encweb = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_=';
    var rawutf8 = toutf8(input), enc = '', pos = 0;
    let len = rawutf8.length;
    while ((len - pos) > 2) {
        enc += encweb[(rawutf8.charCodeAt(pos) >> 2) & 0x3f];
        enc += encweb[((rawutf8.charCodeAt(pos) << 4) | (rawutf8.charCodeAt(pos + 1) >> 4)) & 0x3f];
        enc += encweb[((rawutf8.charCodeAt(pos + 1) << 2) | (rawutf8.charCodeAt(pos + 2) >> 6)) &0x3f];
        enc += encweb[rawutf8.charCodeAt(pos + 2) & 0x3f];
        pos += 3;
    }
    if ((len - pos) == 2) {
        enc += encweb[(rawutf8.charCodeAt(pos) >> 2) & 0x3f];
        enc += encweb[((rawutf8.charCodeAt(pos) << 4) | (rawutf8.charCodeAt(pos + 1) >> 4)) & 0x3f];
        enc += encweb[(rawutf8.charCodeAt(pos + 1) << 2) &0x3f];
        enc += encweb[64];
    }
    if ((len - pos) == 1) {
        enc += encweb[(rawutf8.charCodeAt(pos) >> 2) & 0x3f];
        enc += encweb[(rawutf8.charCodeAt(pos) << 4) & 0x3f];
        enc += encweb[64];
        enc += encweb[64];
    }
    return enc;
}

function run() {
    input_index = 0;
    let script = document.createElement('script');
    let query_string = base64enc(document.getElementById('input').elements['program_source'].value);
    script.setAttribute('src', '/cgi-bin/psc?' + query_string);
    document.body.appendChild(script);
}

function createmenu() {
    var container = document.getElementById('examples');
    if (pseudocode_examples == undefined) {
        container.innerHTML = '<p onclick="alert(\'Could not load pscexamples.js\');">No examples</p>';
        return;
    }
    for (var e = 0; e != pseudocode_examples.length; e++) {
        container.innerHTML += '<p onclick="menuload(\'' + pseudocode_examples[e].name + '\');">' + pseudocode_examples[e].name + '</p>\n';
    }
}

function menuload(program) {
    var container = document.getElementById('input').elements['program_source'];
    if (container.value != '') {
        alert('Please clear input window first!');
        return;
    }
    for (var e = 0; e != pseudocode_examples.length; e++) {
        if ((pseudocode_examples[e].name == program) && (pseudocode_examples[e].code != undefined)) {
            container.value = pseudocode_examples[e].code;
            return;
        }
    }
    alert('Could not load program ' + program);
}

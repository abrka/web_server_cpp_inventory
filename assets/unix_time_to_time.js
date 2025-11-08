const unix_time_elems = document.getElementsByClassName('unix-time');

for (let index = 0; index < unix_time_elems.length; index++) {
  const unix_time_elem = unix_time_elems[index];
  const unix_time = Number(unix_time_elem.textContent);
  const date = new Date(unix_time * 1000);
  unix_time_elem.textContent = date.toLocaleString();
}
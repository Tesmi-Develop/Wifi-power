const buttonConfirm = document.querySelector('#button-confirm');
const inputBoxIp = document.querySelector('#input-box-ip');
const inputBoxPassword = document.querySelector('#input-box-password');

const sendSSIDAndPassword = () => {
  const request = new XMLHttpRequest();
  request.open('POST', '/applySettings', true);
  request.setRequestHeader('Content-Type', 'application/json;charset=UTF-8');

  request.send(JSON.stringify({
    SSID: inputBoxIp.value,
    Password: inputBoxPassword.value,
  }));

  return request;
}

buttonConfirm.addEventListener('click', () => {
  if (inputBoxIp.value != "" && inputBoxPassword.value != "") {
    sendSSIDAndPassword();
  }
});
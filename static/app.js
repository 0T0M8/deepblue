const tbody = document.getElementById("users");
const form = document.getElementById("userForm");
const toastContainer = document.getElementById("toast-container");

/* ================= TOAST ================= */

function showToast(message, type = "info") {
  const toast = document.createElement("div");
  toast.className = `toast ${type}`;
  toast.textContent = message;

  toastContainer.appendChild(toast);
  setTimeout(() => toast.remove(), 4000);
}

/* ================= USERS ================= */

function loadUsers() {
  fetch("/users")
    .then(r => r.json())
    .then(users => {
      tbody.innerHTML = "";
      users.forEach(u => {
        tbody.innerHTML += `
          <tr>
            <td>${u.id}</td>
            <td>${u.name}</td>
            <td>${u.email}</td>
            <td>
              <button onclick="editUser(${u.id}, '${u.name}', '${u.email}')">Edit</button>
              <button class="danger" onclick="deleteUser(${u.id})">Delete</button>
            </td>
          </tr>`;
      });
    })
    .catch(() => showToast("Failed to load users", "error"));
}

/* ================= FORM ================= */

form.onsubmit = e => {
  e.preventDefault();

  const id = document.getElementById("id").value;
  const name = document.getElementById("name").value;
  const email = document.getElementById("email").value;

  if (!name || !email) {
    showToast("Name and email required", "error");
    return;
  }

  const payload = JSON.stringify({ id, name, email });

  fetch("/users", {
    method: id ? "PUT" : "POST",
    headers: { "Content-Type": "application/json" },
    body: payload
  })
    .then(r => r.json())
    .then(res => {
      if (res.status) {
        showToast(
          id ? "User updated successfully" : "User created successfully",
          "success"
        );
        form.reset();
        document.getElementById("id").value = "";
        loadUsers();
      } else {
        showToast(res.error || "Operation failed", "error");
      }
    })
    .catch(() => showToast("Server unreachable", "error"));
};

/* ================= ACTIONS ================= */

function editUser(id, name, email) {
  document.getElementById("id").value = id;
  document.getElementById("name").value = name;
  document.getElementById("email").value = email;
  showToast("Editing user", "info");
}

function deleteUser(id) {
  if (!confirm("Delete user?")) return;

  fetch(`/users?id=${id}`, { method: "DELETE" })
    .then(r => r.json())
    .then(res => {
      if (res.status) {
        showToast("User deleted", "success");
        loadUsers();
      } else {
        showToast(res.error || "Delete failed", "error");
      }
    })
    .catch(() => showToast("Server error", "error"));
}

/* ================= INIT ================= */

loadUsers();

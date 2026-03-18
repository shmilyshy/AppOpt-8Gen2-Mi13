// 加载系统状态
async function loadStatus() {
    try {
        const response = await fetch('/api/status');
        const data = await response.json();
        
        document.getElementById('device-model').textContent = data.device;
        document.getElementById('soc-model').textContent = data.soc;
        
        const statusEl = document.getElementById('appopt-status');
        if (data.appopt_running) {
            statusEl.textContent = '✅ 运行中';
            statusEl.className = 'status-value online';
        } else {
            statusEl.textContent = '❌ 未运行';
            statusEl.className = 'status-value offline';
        }
        
        document.getElementById('cpu-cores').textContent = data.cpu_cores.length + '核';
    } catch (error) {
        console.error('加载状态失败:', error);
    }
}

// 加载应用列表
async function loadApps() {
    try {
        const response = await fetch('/api/apps');
        const data = await response.json();
        
        const appList = document.getElementById('app-list');
        appList.innerHTML = '';
        
        data.apps.forEach(app => {
            const card = document.createElement('div');
            card.className = 'app-card';
            
            const statusClass = app.running ? 'running' : 'stopped';
            const statusText = app.running ? '运行中' : '已停止';
            
            card.innerHTML = `
                <div class="app-header">
                    <div class="app-icon"></div>
                    <div class="app-info">
                        <div class="app-name">${app.name}</div>
                        <div class="app-package">${app.package}</div>
                    </div>
                    <span class="app-status ${statusClass}">${statusText}</span>
                </div>
                <div class="app-affinity">
                    <span class="affinity-label">CPU绑定:</span>
                    <select class="affinity-select" data-package="${app.package}">
                        <option value="0-2" ${app.affinity === '0-2' ? 'selected' : ''}>0-2 (小核)</option>
                        <option value="0-3" ${app.affinity === '0-3' ? 'selected' : ''}>0-3 (小+中)</option>
                        <option value="0-4" ${app.affinity === '0-4' ? 'selected' : ''}>0-4 (小+中)</option>
                        <option value="0-6" ${app.affinity === '0-6' ? 'selected' : ''}>0-6 (全核)</option>
                        <option value="0-7" ${app.affinity === '0-7' ? 'selected' : ''}>0-7 (全核+超大)</option>
                        <option value="3-6" ${app.affinity === '3-6' ? 'selected' : ''}>3-6 (中大核)</option>
                        <option value="7" ${app.affinity === '7' ? 'selected' : ''}>7 (仅超大核)</option>
                    </select>
                    <button class="btn-remove" onclick="removeApp('${app.package}')">删除</button>
                </div>
            `;
            
            appList.appendChild(card);
        });
        
        // 添加选择器变化监听
        document.querySelectorAll('.affinity-select').forEach(select => {
            select.addEventListener('change', saveConfig);
        });
    } catch (error) {
        console.error('加载应用失败:', error);
        document.getElementById('app-list').innerHTML = 
            '<div class="loading">加载失败，请刷新页面重试</div>';
    }
}

// 保存配置
async function saveConfig() {
    const apps = [];
    document.querySelectorAll('.affinity-select').forEach(select => {
        const package = select.dataset.package;
        const affinity = select.value;
        const card = select.closest('.app-card');
        const name = card.querySelector('.app-name').textContent;
        
        apps.push({
            package: package,
            name: name,
            affinity: affinity,
            running: false,
            pid: 0,
            threads: []
        });
    });
    
    try {
        const response = await fetch('/api/save', {
            method: 'POST',
            headers: {'Content-Type': 'application/json'},
            body: JSON.stringify({apps: apps})
        });
        
        const result = await response.json();
        if (result.success) {
            showNotification('配置已保存', 'success');
        } else {
            showNotification('保存失败: ' + result.error, 'error');
        }
    } catch (error) {
        console.error('保存配置失败:', error);
        showNotification('保存失败', 'error');
    }
}

// 删除应用
function removeApp(package) {
    if (confirm('确定要删除这个应用的优化配置吗？')) {
        const card = document.querySelector(`[data-package="${package}"]`).closest('.app-card');
        card.remove();
        saveConfig();
    }
}

// 显示通知
function showNotification(message, type) {
    const notification = document.createElement('div');
    notification.className = `notification ${type}`;
    notification.textContent = message;
    notification.style.cssText = `
        position: fixed;
        top: 20px;
        right: 20px;
        padding: 15px 25px;
        background: ${type === 'success' ? '#4CAF50' : '#f44336'};
        color: white;
        border-radius: 8px;
        box-shadow: 0 4px 12px rgba(0,0,0,0.3);
        z-index: 9999;
        animation: slideIn 0.3s ease;
    `;
    document.body.appendChild(notification);
    setTimeout(() => {
        notification.style.animation = 'slideOut 0.3s ease';
        setTimeout(() => notification.remove(), 300);
    }, 3000);
}

// 打开添加应用对话框
async function openAddAppDialog() {
    try {
        const response = await fetch('/api/installed_apps');
        const data = await response.json();
        
        const dialog = document.createElement('div');
        dialog.className = 'dialog-overlay';
        dialog.innerHTML = `
            <div class="dialog">
                <div class="dialog-header">
                    <h3>添加应用</h3>
                    <button onclick="this.closest('.dialog-overlay').remove()">×</button>
                </div>
                <div class="dialog-body">
                    <input type="text" id="app-search" placeholder="搜索应用..." 
                           style="width:100%;padding:10px;margin-bottom:15px;border:1px solid #ddd;border-radius:8px;">
                    <div id="installed-apps-list" style="max-height:400px;overflow-y:auto;">
                        ${data.apps.map(app => `
                            <div class="installed-app-item" style="padding:10px;border-bottom:1px solid #eee;cursor:pointer;"
                                 onclick="addApp('${app.package}', '${app.name}', '${app.recommended}')">
                                <div style="font-weight:500;">${app.name}</div>
                                <div style="font-size:12px;color:#666;">${app.package}</div>
                                <div style="font-size:12px;color:#999;">推荐: ${app.recommended}</div>
                            </div>
                        `).join('')}
                    </div>
                </div>
            </div>
        `;
        document.body.appendChild(dialog);
        
        // 搜索功能
        document.getElementById('app-search').addEventListener('input', (e) => {
            const search = e.target.value.toLowerCase();
            document.querySelectorAll('.installed-app-item').forEach(item => {
                const text = item.textContent.toLowerCase();
                item.style.display = text.includes(search) ? 'block' : 'none';
            });
        });
    } catch (error) {
        console.error('加载已安装应用失败:', error);
        alert('加载失败，请重试');
    }
}

// 添加应用到配置
function addApp(package, name, affinity) {
    document.querySelector('.dialog-overlay').remove();
    
    const appList = document.getElementById('app-list');
    const card = document.createElement('div');
    card.className = 'app-card';
    
    card.innerHTML = `
        <div class="app-header">
            <div class="app-icon"></div>
            <div class="app-info">
                <div class="app-name">${name}</div>
                <div class="app-package">${package}</div>
            </div>
            <span class="app-status stopped">已停止</span>
        </div>
        <div class="app-affinity">
            <span class="affinity-label">CPU绑定:</span>
            <select class="affinity-select" data-package="${package}">
                <option value="0-2" ${affinity === '0-2' ? 'selected' : ''}>0-2 (小核)</option>
                <option value="0-3" ${affinity === '0-3' ? 'selected' : ''}>0-3 (小+中)</option>
                <option value="0-4" ${affinity === '0-4' ? 'selected' : ''}>0-4 (小+中)</option>
                <option value="0-6" ${affinity === '0-6' ? 'selected' : ''}>0-6 (全核)</option>
                <option value="0-7" ${affinity === '0-7' ? 'selected' : ''}>0-7 (全核+超大)</option>
                <option value="3-6" ${affinity === '3-6' ? 'selected' : ''}>3-6 (中大核)</option>
                <option value="7" ${affinity === '7' ? 'selected' : ''}>7 (仅超大核)</option>
            </select>
            <button class="btn-remove" onclick="removeApp('${package}')">删除</button>
        </div>
    `;
    
    appList.appendChild(card);
    card.querySelector('.affinity-select').addEventListener('change', saveConfig);
    saveConfig();
    showNotification('应用已添加', 'success');
}

// 初始化
loadStatus();
loadApps();

// 定时刷新
setInterval(loadStatus, 5000);
setInterval(loadApps, 10000);

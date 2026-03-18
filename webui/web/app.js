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
                    <span class="affinity-value">${app.affinity || '未配置'}</span>
                </div>
            `;
            
            appList.appendChild(card);
        });
    } catch (error) {
        console.error('加载应用失败:', error);
        document.getElementById('app-list').innerHTML = 
            '<div class="loading">加载失败，请刷新页面重试</div>';
    }
}

// 初始化
loadStatus();
loadApps();

// 定时刷新
setInterval(loadStatus, 5000);
setInterval(loadApps, 10000);

// Execute the actual embedded JavaScript with a small DOM/network test double.
const fs = require('node:fs');
const path = require('node:path');
const vm = require('node:vm');
const assert = require('node:assert/strict');
const source = fs.readFileSync(path.join(__dirname, '../MaintenanceService.cpp'), 'utf8');
assert(!source.includes('\uFFFD'),'Maintenance source must be valid UTF-8');
const html = source.match(/R"HTML\(([\s\S]*?)\)HTML"/)[1];
const script = html.match(/<script>([\s\S]*?)<\/script>/)[1];
class Element {
  constructor(tag, attrs={}) { this.tagName=tag; Object.assign(this, attrs); this.dataset={}; this.children=[]; this.textContent=''; this.value=''; this.type=attrs.type||''; this.checked=false; this.hidden=false; }
  append(...nodes) { this.children.push(...nodes); }
  appendChild(node) { this.append(node); }
  replaceChildren(...nodes) { this.children=nodes; }
  setAttribute(key,value) { this[key]=value; }
  querySelector(selector) { if(selector==='.feedback') return this.feedback; throw Error(selector); }
}
function attrs(text) { return Object.fromEntries([...text.matchAll(/([\w-]+)="([^"]*)"/g)].map(m=>[m[1],m[2]])); }
const ids = new Map();
for(const m of html.matchAll(/<(\w+)\b([^>]*)>/g)) { const a=attrs(m[2]); if(a.id) { assert(!ids.has(a.id),'duplicate ID '+a.id); ids.set(a.id,new Element(m[1],a)); } }
const forms=[...html.matchAll(/<form class="settings card" data-section="([^"]+)">([\s\S]*?)<\/form>/g)].map(m=>{
  const f=new Element('form');f.dataset.section=m[1]; f.feedback=new Element('p');
  f.elements=[...m[2].matchAll(/<(input|select|textarea)\b([^>]*)>/g)].map(x=>new Element(x[1],attrs(x[2])));
  for(const e of f.elements)f.elements[e.name]=e;
  return f;
});
const tabs=[...html.matchAll(/<button type="button" data-tab="([^"]+)"/g)].map(m=>{let b=new Element('button');b.dataset.tab=m[1];return b;});
const settings={ssid:'Test Phone',incomingMode:1,outgoingMode:2,privatePolicy:2,unknownPolicy:0,incomingList:'2125550123',outgoingList:'9005550123',block911:1,blockPremium:1,rejectShort:1,minimumDigits:7,scheduledDnd:1,manualDnd:0,dndStart:'22:00',dndEnd:'06:00',timezone:'EST5EDT,M3.2.0/2,M11.1.0/2',powerSaving:1,pollMs:500,dndActive:1,clockValid:1,historyReady:1};
const posts=[];
let history=[{incoming:true,number:'2125550199',identity:'number',utc:0,outcome:'missed',connected:false,silenced:true},{incoming:false,number:'<script>bad</script>',identity:'number',utc:0,outcome:'blocked',connected:false,silenced:false},{incoming:true,number:'2125550133',identity:'number',utc:1791662400,localTime:'2026-10-10 16:00:00 EDT',outcome:'completed',connected:true,silenced:false}];
const document={getElementById:id=>{assert(ids.has(id),'missing ID '+id);return ids.get(id);},createElement:tag=>new Element(tag),querySelectorAll:selector=>{
 if(selector==='form.settings')return forms;if(selector==='[data-tab]')return tabs;if(selector==='section[id^="tab-"]')return [...ids.values()].filter(e=>e.id.startsWith('tab-'));throw Error(selector);
},querySelector:selector=>{
 let m=selector.match(/^\[name="([^"]+)"\]$/);if(m)return forms.flatMap(f=>f.elements).find(e=>e.name===m[1]);
 m=selector.match(/^\[data-tab="([^"]+)"\]$/);if(m)return tabs.find(e=>e.dataset.tab===m[1]);throw Error(selector);
}};
const fetch=async(url,options={})=>{
 if(options.method==='POST')posts.push({url,body:options.body});
 let data=url==='/api/settings'?settings:url==='/api/history'?history:url==='/api/power-files'?[]:{version:'0.12.0-dev',wifiTimeoutEnabled:true,wifiSecondsRemaining:600};
 return {ok:true,json:async()=>data,text:async()=>options.method==='POST'?'Saved':'',headers:{get:()=>0}};
};
const context=vm.createContext({document,fetch,URLSearchParams,Date,console,setInterval:()=>{},confirm:()=>true,alert:()=>{}});
vm.runInContext(script,context);
async function drain(){for(let i=0;i<12;++i)await Promise.resolve();}
(async()=>{
 await drain();
 assert.equal(forms.length,4);assert.equal(tabs.length,6);
 const calls=forms.find(f=>f.dataset.section==='calls'),phone=forms.find(f=>f.dataset.section==='phone'),wifi=forms.find(f=>f.dataset.section==='wifi');
 assert.equal(calls.elements.incomingMode.value,1);assert.equal(calls.elements.block911.checked,true);
 assert.equal(phone.elements.dndStart.value,'22:00');assert.equal(wifi.elements.password.value,'');
 assert.match(ids.get('dndSummary').textContent,/active/);
 tabs.find(t=>t.dataset.tab==='calls').onclick();await drain();assert.equal(ids.get('tab-calls').hidden,false);assert.equal(ids.get('tab-system').hidden,true);
 const table=ids.get('history').children[0].children[0];
 assert.equal(table.children[1].children[2].textContent,'Time unavailable');
 assert.equal(table.children[2].children[1].textContent,'<script>bad</script>'); // text, never markup
  assert.equal(table.children[3].children[2].textContent,'2026-10-10 16:00:00 EDT','history should use the phone time zone');
 const add=table.children[1].children[4].children[0];add.onclick();add.onclick();
 assert.equal(calls.elements.incomingList.value,'2125550123\n2125550199');
  await vm.runInContext('summary()',context);
  assert.equal(calls.elements.incomingList.value,'2125550123\n2125550199','refresh must preserve unsaved history additions');
  settings.manualDnd=1;await vm.runInContext('summary()',context);
  assert.equal(phone.elements.manualDnd.checked,true,'service-code changes must reach the settings form');
  assert.match(ids.get('dndSummary').textContent,/until turned off/);
 calls.elements.block911.checked=false;await calls.onsubmit({preventDefault(){}});
 const post=posts.findLast(p=>p.url==='/api/settings');assert.equal(post.body.get('block911'),'0');assert.equal(post.body.get('section'),'calls');
 assert.equal(post.body.get('incomingList'),'2125550123\n2125550199');
 settings.clockValid=0;await vm.runInContext('summary()',context);assert.match(ids.get('dndSummary').textContent,/Schedule paused/);
 wifi.elements.password.value='newpassword';await wifi.onsubmit({preventDefault(){}});assert.equal(wifi.elements.password.value,'');
 assert(!posts.some(p=>p.url==='/api/wifi/apply'),'saving must not disconnect Wi-Fi');
 await ids.get('applyWifi').onclick();assert(posts.some(p=>p.url==='/api/wifi/apply'));
 await ids.get('clearHistory').onclick();assert(posts.some(p=>p.url==='/api/history/clear'));
 console.log('PASS: navigation, settings population, unchecked restrictions, safe history rendering, list selection, invalid-clock status, password handling, explicit Wi-Fi apply, clear history');
})().catch(e=>{console.error(e);process.exitCode=1;});

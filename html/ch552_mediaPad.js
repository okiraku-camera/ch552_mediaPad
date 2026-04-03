/**
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2026 Takeshi Higasa, okiraku-camera.tokyo.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/** 1. 定数・データ定義 **/
const HID_CONFIG = {
  VENDOR_ID: 0x1209,
  USAGE_PAGE: 0xff82,
  USAGE_ID: 0x51,
  REPORT_ID: 5,
  RAW_DATA_SIZE: 32,
  VERSION_NUM: 0x1234,
  LAYER_COUNT: 2,
  KEYMAP_SIZE: 8,
  MACRO_SIZE: 85,  
  MACRO_COUNT: 10,
  NVM_SIZE: 128,
  COMMAND_TIMEOUT: 5000
};
// MACRO_SIZEは実メモリ量より1バイト少なくしている。。

const HID_COMMANDS = {
  GET_VERSION: 0x01,
  READ_NVM: 0x0a,
  READ_MACRO_PTRS: 0x0b,
  START_BOOT_LOADER: 0x0c,
  RESET_KEYMAP: 0x10,
  READ_KEYMAP: 0x15,
  WRITE_KEYMAP: 0x16,
  READ_MACROS: 0x18,
  WRITE_MACROS: 0x19,
  WRITE_RE_KEY: 0x1c,
  READ_RE_KEY: 0x1d
};

const FN_SWITCH_ID = 7;
const FN_USAGE = 0x00BC;

// --- ダイアログ用キー定義 ---
const charKeyDefs = [
  { label: "a", usage: 0x04 }, { label: "b", usage: 0x05 }, { label: "c", usage: 0x06 }, { label: "d", usage: 0x07 }, { label: "e", usage: 0x08 },
  { label: "f", usage: 0x09 }, { label: "g", usage: 0x0A }, { label: "h", usage: 0x0B }, { label: "i", usage: 0x0C }, { label: "j", usage: 0x0D },
  { label: "k", usage: 0x0E }, { label: "l", usage: 0x0F }, { label: "m", usage: 0x10 }, { label: "n", usage: 0x11 }, { label: "o", usage: 0x12 },
  { label: "p", usage: 0x13 }, { label: "q", usage: 0x14 }, { label: "r", usage: 0x15 }, { label: "s", usage: 0x16 }, { label: "t", usage: 0x17 },
  { label: "u", usage: 0x18 }, { label: "v", usage: 0x19 }, { label: "w", usage: 0x1A }, { label: "x", usage: 0x1B }, { label: "y", usage: 0x1C }, { label: "z", usage: 0x1D },
  { label: "1", usage: 0x1E }, { label: "2", usage: 0x1F }, { label: "3", usage: 0x20 }, { label: "4", usage: 0x21 }, { label: "5", usage: 0x22 },
  { label: "6", usage: 0x23 }, { label: "7", usage: 0x24 }, { label: "8", usage: 0x25 }, { label: "9", usage: 0x26 }, { label: "0", usage: 0x27 },
  { label: "space", usage: 0x2C },
  { label: "-", usage: 0x2D }, { label: "^", usage: 0x2E }, { label: "@", usage: 0x2F }, { label: "[", usage: 0x30 },
  { label: "]", usage: 0x31 }, { label: ";", usage: 0x33 }, { label: ":", usage: 0x34 },
  { label: "`", usage: 0x35 }, { label: ",", usage: 0x36 }, { label: ".", usage: 0x37 }, { label: "/", usage: 0x38 },
  { label: "\\", usage: 0x64 }
];
const editKeyDefs = [
  { label: "BackSp", usage: 0x2A }, { label: "Tab", usage: 0x2B }, { label: "Enter", usage: 0x28 }, { label: "Esc", usage: 0x29 },
  { label: "Del", usage: 0x4C }, { label: "Ins", usage: 0x49 }, { label: "Home", usage: 0x4A }, { label: "End", usage: 0x4D },
  { label: "PgUp", usage: 0x4B }, { label: "PgDn", usage: 0x4E }, { label: "PrtScr", usage: 0x46 },
  { label: "ScrLock", usage: 0x47 }, { label: "Pause", usage: 0x48 }, { label: "Caps", usage: 0x39 },
  { label: "↑", usage: 0x52 }, { label: "↓", usage: 0x51 }, { label: "←", usage: 0x50 }, { label: "→", usage: 0x4F }
];

const japaneseExtKeyDefs = [
  { label: "ひらがな", usage: 0x88 },
  { label: "無変換", usage: 0x8B },
  { label: "変換", usage: 0x8A },
  { label: " _ ", usage: 0x87 },
  { label: "LANG1(かな)", usage: 0x90 },
  { label: "LANG2(英数)", usage: 0x91 }
];

const specialKeyDefs = [
  { label: "<<", usage: 0xA5 },
  { label: "Play", usage: 0xA0 },
  { label: ">>", usage: 0xA4 },
  { label: "VolUp", usage: 0xA7 },
  { label: "VolDown", usage: 0xA8 },
  { label: "Mute", usage: 0xA9 }
];

const mouseSimKeyDefs = [
  { label: "左ボタン", usage: 0xC4 },
  { label: "右ボタン", usage: 0xC5 },
  { label: "M ←", usage: 0xC7 },
  { label: "M →", usage: 0xC8 },
  { label: "M ↑", usage: 0xC9 },
  { label: "M ↓", usage: 0xCA },
  { label: "M ⇧", usage: 0xCB },
  { label: "M ⇩", usage: 0xCC }
];

const functionKeyDefs = [
  ...Array.from({ length: 12 }, (_, i) => ({ label: `F${i + 1}`, usage: 0x3A + i })),
  ...Array.from({ length: 12 }, (_, i) => ({ label: `F${i + 13}`, usage: 0x68 + i }))
];

const modifierKeyDefs = [
  { key: "shift", label: "Shift", bit: 0x0200 },
  { key: "ctrl", label: "Ctrl", bit: 0x0100 },
  { key: "alt", label: "Alt", bit: 0x0400 },
  { key: "win", label: "Win", bit: 0x0800 }
];

const shiftedCharLabelMap = {
  0x1E: "!",
  0x1F: "\"",
  0x20: "#",
  0x21: "$",
  0x22: "%",
  0x23: "&",
  0x24: "'",
  0x25: "(",
  0x26: ")",
  0x2D: "=",
  0x2E: "~",
  0x2F: "`",
  0x30: "{",
  0x31: "}",
  0x33: "+",
  0x34: "*",
  0x35: "~",
  0x36: "<",
  0x37: ">",
  0x38: "?",
  0x64: "_"
};

// --- マクロキー ---
const macroKeyDefs = Array.from({length:10}, (_,i)=>({label:`m${i}`, usage:0xD0 + i}));

const keypadUsageMap = {
  0x53: "NumLock",
  0x54: "KP /",
  0x55: "KP *",
  0x56: "KP -",
  0x57: "KP +",
  0x58: "KP Enter",
  0x59: "KP 1",
  0x5A: "KP 2",
  0x5B: "KP 3",
  0x5C: "KP 4",
  0x5D: "KP 5",
  0x5E: "KP 6",
  0x5F: "KP 7",
  0x60: "KP 8",
  0x61: "KP 9",
  0x62: "KP 0",
  0x63: "KP .",
  0x67: "KP =",
  0x85: "KP ,"
};


// --- 物理レイアウト (2列8行 / Switch ID 0~15) ---
const PHYSICAL_LAYOUT = Array.from({ length: HID_CONFIG.KEYMAP_SIZE }, (_, id) => ({
  id,
  row: Math.floor(id / 2),
  col: id % 2
}));

// --- 読み出し時のラベル変換用マップ ---
const MASTER_USAGE_MAP = { 0xBC: "Fn", 0x9C: "Clear" };
[...charKeyDefs, ...editKeyDefs, ...japaneseExtKeyDefs, ...specialKeyDefs, ...mouseSimKeyDefs, ...functionKeyDefs, ...macroKeyDefs].forEach(d => MASTER_USAGE_MAP[d.usage] = d.label);
Object.assign(MASTER_USAGE_MAP, keypadUsageMap);

const toHex2 = (value) => `0x${value.toString(16).toUpperCase().padStart(2, "0")}`;

const MACRO_USAGE_LABELS = (() => {
  // MASTER_USAGE_MAP をベースに、マクロエディタ用に追加・上書きする
  const map = Object.assign({}, MASTER_USAGE_MAP);

  Object.assign(map, {
    // charKeyDefs は JP レイアウト前提のため HID 標準ラベルで上書き
    0x2A: "BackSP",
    0x2C: "Space",
    0x2E: "=",
    0x2F: "[",
    0x30: "]",
    0x31: "\\",
    0x32: "0x32",
    0x34: "'",
    0x46: "PrtSc",
    0x64: "NonUS\\",
    // 追加テンキー
    ...keypadUsageMap,
    // その他
    0x65: "App",
    0x9A: "SysReq",
    // ディレイコード
    0xDA: "100ms",
    0xDB: "500ms",
    0xDC: "1sec",
    // 修飾キー
    0xE0: "L-Ctrl",
    0xE1: "L-Shift",
    0xE2: "L-Alt",
    0xE3: "L-GUI",
    0xE4: "R-Ctrl",
    0xE5: "R-Shift",
    0xE6: "R-Alt",
    0xE7: "R-GUI",
    // japaneseExtKeyDefs より短いラベルに上書き
    0x90: "LANG1",
    0x91: "LANG2"
  });

  // Intl / Lang キー（MASTER_USAGE_MAP にないものを補完）
  for (let usage = 0x87; usage <= 0x8F; usage++) {
    if (!(usage in map)) map[usage] = `Intl${usage - 0x86}`;
  }
  for (let usage = 0x90; usage <= 0x98; usage++) {
    if (!(usage in map)) map[usage] = `Lang${usage - 0x8F}`;
  }
  for (let usage = 0xE8; usage <= 0xEF; usage++) {
    map[usage] = `Reserved ${toHex2(usage)}`;
  }
  return map;
})();

const getMacroUsageLabel = (usage) => MACRO_USAGE_LABELS[usage] || `Usage ${toHex2(usage)}`;

const isMacroBasicUsageAllowed = (usage) => {
  if (usage < 0x04 || usage > 0x9F) {
    return false;
  }
  if (usage === 0x64 || usage === 0x65) {
    return false;
  }
  if (usage >= 0x74 && usage <= 0x9F) {
    return false;
  }
  return true;
};

const JP_MACRO_USAGES = new Set([0x88, 0x8A, 0x8B, 0x90, 0x91]);
const DELAY_MACRO_USAGES = new Set([0xDA, 0xDB, 0xDC]);
const isMacroEditableUsage = (usage) => (isMacroBasicUsageAllowed(usage) || (usage >= 0xE0 && usage <= 0xE3) || JP_MACRO_USAGES.has(usage) || DELAY_MACRO_USAGES.has(usage));

const macroBasicUsageDefs = Array.from({ length: 0x9F - 0x04 + 1 }, (_, i) => {
  const usage = 0x04 + i;
  return { usage, label: getMacroUsageLabel(usage) };
}).filter((d) => isMacroBasicUsageAllowed(d.usage));

const macroModifierUsageDefs = Array.from({ length: 0xE3 - 0xE0 + 1 }, (_, i) => {
  const usage = 0xE0 + i;
  return { usage, label: getMacroUsageLabel(usage) };
});

const macroJpKeyDefs = [0x88, 0x8A, 0x8B, 0x90, 0x91].map(usage => ({ usage, label: getMacroUsageLabel(usage) }));
const macroDelayKeyDefs = [0xDA, 0xDB, 0xDC].map((usage) => ({ usage, label: getMacroUsageLabel(usage) }));

/** 2. 状態管理 **/
let fnToggleOn = false;
let viewMode = 0; // Fn OFF: layer 0 / Fn ON: layer 1
let selectedSwitchId = null;
let selectedEncoderDirection = null; // "cw" | "ccw" | null
let selectedMacroIndex = 0;
let device = null;
let commandResolver = null;
let hasUnsavedMacroEdits = false;
let selectedModifiers = {
  shift: false,
  ctrl: false,
  alt: false,
  win: false
};
let keymaps = Array.from({ length: HID_CONFIG.LAYER_COUNT }, () =>
  new Array(HID_CONFIG.KEYMAP_SIZE).fill(null).map(() => ({ usage: 0, label: "None" }))
);

let encoderKeymaps = Array.from({ length: HID_CONFIG.LAYER_COUNT }, () => ({
  cw: { usage: 0, label: "None" },
  ccw: { usage: 0, label: "None" }
}));

// ローカルで変更されたが未書き込みのキーを追跡
let pendingChanges = new Set(); // "layer:switchId" 形式の文字列を格納
let pendingEncoderChanges = new Set(); // "layer:cw|ccw"

// マクロデータ: 各マクロはUsageIDの配列
let macros = new Array(HID_CONFIG.MACRO_COUNT).fill(null).map(() => []);

function syncViewModeWithFnToggle() {
  if (HID_CONFIG.LAYER_COUNT < 2) {
    viewMode = 0;
    return;
  }
  viewMode = fnToggleOn ? 1 : 0;
}

function enforceFixedFnSwitchAllLayers() {
  for (let layer = 0; layer < HID_CONFIG.LAYER_COUNT; layer++) {
    keymaps[layer][FN_SWITCH_ID] = { usage: FN_USAGE, label: "Fn" };
  }
}

const CONNECTED_COMMAND_BUTTON_IDS = [
  "readKeymapBtn",
  "writeKeymapBtn",
  "readMacrosBtn",
  "writeMacrosBtn",
  "openMacroEditorBtn",
  "resetKeymapBtn",
  "startBootloaderBtn",
  "readNvmBtn",
  "readMacroPtrsBtn",
  "macroDeleteLastBtn",
  "macroClearBtn"
];

function setConnectedUiState(connected) {
  CONNECTED_COMMAND_BUTTON_IDS.forEach((id) => {
    const el = document.getElementById(id);
    if (el) {
      el.disabled = !connected;
    }
  });
}

function clearConnectionState(reason = "Device disconnected.", options = {}) {
  const { log = true } = options;
  if (commandResolver) {
    const { reject, timer } = commandResolver;
    clearTimeout(timer);
    commandResolver = null;
    reject(new Error(reason));
  }
  if (device) {
    try {
      device.oninputreport = null;
    } catch (_e) {
      // ignore
    }
  }
  device = null;
  setConnectedUiState(false);
  document.getElementById("keyDialog")?.classList.add("hidden");
  document.getElementById("specialKeyDialog")?.classList.add("hidden");
  document.getElementById("macroEditorDialog")?.classList.add("hidden");
  if (log) {
    updateLog(reason);
  }
}

function isLikelyDisconnectError(error) {
  if (!error) return false;
  const name = String(error.name || "").toLowerCase();
  const msg = String(error.message || "").toLowerCase();
  return (
    name.includes("network") ||
    name.includes("invalidstate") ||
    msg.includes("not connected") ||
    msg.includes("disconnected") ||
    msg.includes("closed")
  );
}

function handleHidDisconnect(event) {
  if (!device || event.device !== device) return;
  clearConnectionState("Device disconnected or powered off.");
}

function cloneMacros(source = macros) {
  return source.map((entry) => (Array.isArray(entry) ? entry.slice() : []));
}

function normalizeMacroOrder(source = macros) {
  const packed = [];
  for (let i = 0; i < HID_CONFIG.MACRO_COUNT; i++) {
    const entry = Array.isArray(source[i]) ? source[i].slice() : [];
    if (entry.length > 0) {
      packed.push(entry);
    }
  }
  while (packed.length < HID_CONFIG.MACRO_COUNT) {
    packed.push([]);
  }
  return packed;
}

function getFirstEmptyMacroIndex(source = macros) {
  for (let i = 0; i < HID_CONFIG.MACRO_COUNT; i++) {
    if (!source[i] || source[i].length === 0) {
      return i;
    }
  }
  return HID_CONFIG.MACRO_COUNT;
}

function getMacroSelectableLimit(source = macros) {
  return Math.min(getFirstEmptyMacroIndex(source), HID_CONFIG.MACRO_COUNT - 1);
}

function getMacroStorageUsedBytes(source = macros) {
  let used = 0;
  for (let i = 0; i < HID_CONFIG.MACRO_COUNT; i++) {
    const macro = source[i] || [];
    if (macro.length === 0) {
      break;
    }
    used += macro.length + 1;
  }
  return used;
}

function getMacroRemainingBytes(source = macros) {
  return Math.max(0, HID_CONFIG.MACRO_SIZE - getMacroStorageUsedBytes(source));
}

function setMacrosIfFits(nextMacros, options = {}) {
  const normalized = normalizeMacroOrder(nextMacros);
  const usedBytes = getMacroStorageUsedBytes(normalized);
  if (usedBytes > HID_CONFIG.MACRO_SIZE) {
    if (!options.silent) {
      updateLog(`Macro area is full. Remaining bytes: ${getMacroRemainingBytes(macros)}`);
    }
    return false;
  }
  macros = normalized;
  globalThis.macros = macros;
  const limit = getMacroSelectableLimit(macros);
  if (selectedMacroIndex > limit) {
    selectedMacroIndex = limit;
  }
  return true;
}

function appendMacroUsageToSelected(usage) {
  if (!isMacroEditableUsage(usage)) {
    updateLog(`Unsupported macro usage: ${toHex2(usage)}`);
    return;
  }
  const next = cloneMacros();
  next[selectedMacroIndex].push(usage);
  if (setMacrosIfFits(next)) {
    hasUnsavedMacroEdits = true;
    renderMacroEditor();
  }
}

function removeMacroCodeAt(macroIndex, codeIndex) {
  const next = cloneMacros();
  if (!next[macroIndex] || codeIndex < 0 || codeIndex >= next[macroIndex].length) {
    return;
  }
  next[macroIndex].splice(codeIndex, 1);
  if (setMacrosIfFits(next, { silent: true })) {
    hasUnsavedMacroEdits = true;
    renderMacroEditor();
  }
}

function removeLastMacroCode() {
  const current = macros[selectedMacroIndex] || [];
  if (current.length === 0) {
    return;
  }
  removeMacroCodeAt(selectedMacroIndex, current.length - 1);
}

function clearSelectedMacro() {
  const next = cloneMacros();
  next[selectedMacroIndex] = [];
  if (setMacrosIfFits(next, { silent: true })) {
    hasUnsavedMacroEdits = true;
    renderMacroEditor();
  }
}


/** 3. WebHID 通信 **/
async function sendCommand(cmdId, payload = null) {
  if (!device?.opened) throw new Error("Device not connected");
  const data = new Uint8Array(HID_CONFIG.RAW_DATA_SIZE);
  data[0] = cmdId;
  if (payload) data.set(payload, 1);

  // sendReport の Promise を完全に待つ必要がある（複数チャンク送信時の競合防止）
  try {
    await device.sendReport(HID_CONFIG.REPORT_ID, data);
  } catch (e) {
    if (isLikelyDisconnectError(e)) {
      clearConnectionState("Device disconnected or powered off.", { log: true });
    }
    throw new Error(`Send failed: ${e.message}`);
  }

  // デバイスからの応答を待つ
  return new Promise((resolve, reject) => {
    const timer = setTimeout(() => {
      commandResolver = null;
      reject(new Error(`Timeout (0x${cmdId.toString(16)})`));
    }, HID_CONFIG.COMMAND_TIMEOUT);
    commandResolver = { resolve, reject, timer };
  });
}

// マクロ読み出し
async function readMacros(force = false) {
  if (!device) return;
  if (hasUnsavedMacroEdits && !force) {
    updateLog("Skip READ_MACROS: unsaved local macro edits exist.");
    return;
  }
  try {
    updateLog("Reading macros...");
    const macroData = new Uint8Array(HID_CONFIG.MACRO_SIZE).fill(0);
    const payload = new Uint8Array(HID_CONFIG.RAW_DATA_SIZE - 1);
    const dataSize = payload.length - 2;

    for (let start = 0; start < HID_CONFIG.MACRO_SIZE; start += dataSize) {
      const chunkSize = Math.min(dataSize, HID_CONFIG.MACRO_SIZE - start);
      payload.fill(0);
      payload[0] = start;
      payload[1] = chunkSize;
      const { body } = await sendCommand(HID_COMMANDS.READ_MACROS, payload);
      const readBytes = Math.min(body[0], chunkSize);
      macroData.set(body.slice(2, 2 + readBytes), start);
    }

    const parsed = new Array(HID_CONFIG.MACRO_COUNT).fill(null).map(() => []);
    let offset = 0;
    for (let i = 0; i < HID_CONFIG.MACRO_COUNT && offset < HID_CONFIG.MACRO_SIZE; i++) {
      while (offset < HID_CONFIG.MACRO_SIZE && macroData[offset] !== 0) {
        parsed[i].push(macroData[offset++]);
      }
      if (parsed[i].length === 0) {
        break;
      }
      offset++; // skip \0
    }

    setMacrosIfFits(parsed, { silent: true });
    hasUnsavedMacroEdits = false;
    renderMacroEditor();
    updateLog("Macros read.");
  } catch (e) {
    updateLog(`Macro read error: ${e.message}`);
  }
}

// マクロ書き込み
// 各マクロ配列は 0 を含まない前提
async function writeMacros() {
  if (!device) return;
  try {
    setMacrosIfFits(cloneMacros(), { silent: true });
    updateLog("Writing macros...");
    const macrodata = new Uint8Array(HID_CONFIG.MACRO_SIZE).fill(0);
    let offset = 0;
    
    // マクロデータを結合（バッファオーバーフロー防止）
    for (let i = 0; i < HID_CONFIG.MACRO_COUNT; i++) {
      if (macros[i].length === 0) break;
      
      // このマクロのすべての要素をコピー
      for (const u of macros[i]) {
        if (offset >= HID_CONFIG.MACRO_SIZE - 1) {  // -1: ターミネータ用に予約
          console.warn(`Macro data overflow: macro ${i} at offset ${offset}, buffer is ${HID_CONFIG.MACRO_SIZE} bytes`);
          updateLog(`Warning: Macro data too large, truncated at macro ${i}`);
          offset = HID_CONFIG.MACRO_SIZE;  // バッファを満杯にして終了
          i = HID_CONFIG.MACRO_COUNT;  // 外側のループも終了
          break;
        }
        macrodata[offset++] = u;
      }
      
      // このマクロの終端マーク
      if (offset >= HID_CONFIG.MACRO_SIZE) {
        console.warn(`Macro data overflow: no space for terminator at offset ${offset}`);
        break;
      }
      macrodata[offset++] = 0; // ターミネータ \0
    }
    
    const payload = new Uint8Array(HID_CONFIG.RAW_DATA_SIZE - 1);
    const datasize = payload.length - 2; // 2 bytes for offset and length
    for(let start = 0; start < HID_CONFIG.MACRO_SIZE; start += datasize) {
      let left = HID_CONFIG.MACRO_SIZE - start;
      const chunkSize = Math.min(datasize, left);
      payload.fill(0);
      payload.set(macrodata.slice(start, start + chunkSize), 2);
      payload[0] = start;
      payload[1] = chunkSize;
      await sendCommand(HID_COMMANDS.WRITE_MACROS, payload);
    }
    hasUnsavedMacroEdits = false;
    updateLog("Macros written.");
  } catch (e) {
    updateLog(`Macro write error: ${e.message}`);
  }
}

async function readEncoderKeysForLayer(layer) {
  const requestedEncoderCount = 1; // RE0 only
  const requiredBufferBytes = requestedEncoderCount * 4; // 1 encoder = CW(2) + CCW(2)
  const payload = new Uint8Array([0, layer, requestedEncoderCount]); // RE0, layer, encoder count
  const { body } = await sendCommand(HID_COMMANDS.READ_RE_KEY, payload);
  // device side: data[1]=read encoder count, data[4..] = keycodes (4 bytes per encoder)
  const readEncoderCount = body[0] || 0;
  if (readEncoderCount < requestedEncoderCount) {
    throw new Error(`READ_RE_KEY returned ${readEncoderCount} encoder(s)`);
  }
  if (requiredBufferBytes > (HID_CONFIG.RAW_DATA_SIZE - 4)) {
    throw new Error("READ_RE_KEY buffer size exceeds report capacity");
  }
  const cw = (body[3] || 0) | ((body[4] || 0) << 8);
  const ccw = (body[5] || 0) | ((body[6] || 0) << 8);
  return { cw, ccw };
}

async function writeEncoderKeysForLayer(layer, reCount, cwUsage, ccwUsage) {
  const payload = new Uint8Array([
    0,
    layer,
    reCount,
    cwUsage & 0xFF,
    (cwUsage >> 8) & 0xFF,
    ccwUsage & 0xFF,
    (ccwUsage >> 8) & 0xFF
  ]);
  const { body } = await sendCommand(HID_COMMANDS.WRITE_RE_KEY, payload);
  return body[0] || 0;
}

function setEncoderLabelForLayer(layer, cw, ccw) {
  const cwLo = cw & 0xFF;
  const ccwLo = ccw & 0xFF;
  encoderKeymaps[layer].cw = {
    usage: cw,
    label: buildLabelWithModifiers(MASTER_USAGE_MAP[cwLo] || `0x${cwLo.toString(16).toUpperCase()}`, getModifierNamesFromUsage(cw))
  };
  encoderKeymaps[layer].ccw = {
    usage: ccw,
    label: buildLabelWithModifiers(MASTER_USAGE_MAP[ccwLo] || `0x${ccwLo.toString(16).toUpperCase()}`, getModifierNamesFromUsage(ccw))
  };
}

async function syncEncoderKeysFromDevice() {
  for (let layer = 0; layer < HID_CONFIG.LAYER_COUNT; layer++) {
    const { cw, ccw } = await readEncoderKeysForLayer(layer);
    setEncoderLabelForLayer(layer, cw, ccw);
  }
}

function handleInputReport({ reportId, data }) {
  if (reportId !== HID_CONFIG.REPORT_ID || !commandResolver) return;
  const arr = new Uint8Array(data.buffer);
  const { resolve, timer } = commandResolver;
  clearTimeout(timer);
  commandResolver = null;
  resolve({ cmd: arr[0], body: arr.slice(1) });
}

/** 4. UI 描画・ログ **/
const updateLog = (msg) => {
  const logEl = document.getElementById("log");
  if (!logEl) return;
  logEl.textContent += `\n> ${msg}`;
  logEl.scrollTop = logEl.scrollHeight;
};

function getSelectedModifierBits() {
  let bits = 0;
  modifierKeyDefs.forEach((def) => {
    if (selectedModifiers[def.key]) {
      bits |= def.bit;
    }
  });
  return bits;
}

function getSelectedModifierNames() {
  return modifierKeyDefs
    .filter((def) => selectedModifiers[def.key])
    .map((def) => def.label);
}

function getCharKeyDisplayLabel(def) {
  if (!selectedModifiers.shift) {
    return def.label;
  }
  if (def.usage >= 0x04 && def.usage <= 0x1D) {
    return def.label.toUpperCase();
  }
  return shiftedCharLabelMap[def.usage] || def.label;
}

function buildLabelWithModifiers(baseLabel, modifierNames) {
  if (!modifierNames || modifierNames.length === 0) {
    return baseLabel;
  }
  return `${modifierNames.join("+")}+${baseLabel}`;
}

function getModifierNamesFromUsage(usage) {
  const hi = (usage >> 8) & 0xFF;
  const names = [];
  if (hi & 0x11) names.push("Ctrl");
  if (hi & 0x22) names.push("Shift");
  if (hi & 0x44) names.push("Alt");
  if (hi & 0x88) names.push("Win");
  return names;
}

function hasFullWidthCharacter(text = "") {
  return /[^\u0000-\u00FF]/.test(text);
}

function escapeHtml(text) {
  return String(text)
    .replace(/&/g, "&amp;")
    .replace(/</g, "&lt;")
    .replace(/>/g, "&gt;")
    .replace(/\"/g, "&quot;")
    .replace(/'/g, "&#39;");
}

function getBaseLabelFromUsageForFace(usage, fallbackLabel = "") {
  const usageLo = usage & 0xFF;
  const hi = (usage >> 8) & 0xFF;
  const hasShift = (hi & 0x22) !== 0;

  const charDef = charKeyDefs.find((d) => d.usage === usageLo);
  if (charDef) {
    if (hasShift) {
      if (usageLo >= 0x04 && usageLo <= 0x1D) {
        return charDef.label.toUpperCase();
      }
      return shiftedCharLabelMap[usageLo] || charDef.label;
    }
    return charDef.label;
  }

  return MASTER_USAGE_MAP[usageLo] || fallbackLabel || `0x${usageLo.toString(16).toUpperCase()}`;
}

function isMacroTriggerUsage(usage) {
  return usage >= 0xD0 && usage <= 0xD9;
}

function populateMacroUsageButtons(containerId, defs) {
  const area = document.getElementById(containerId);
  if (!area) return;
  area.innerHTML = "";
  defs.forEach((d) => {
    const b = document.createElement("button");
    b.textContent = d.label;
    b.onclick = () => appendMacroUsageToSelected(d.usage);
    area.appendChild(b);
  });
}

function renderMacroStorageInfo() {
  const used = getMacroStorageUsedBytes(macros);
  const remaining = getMacroRemainingBytes(macros);
  const statusEl = document.getElementById("macroStorageStatus");
  const remainingEl = document.getElementById("macroRemainingBytes");
  if (statusEl) {
    const dirtyTag = hasUnsavedMacroEdits ? " (未保存)" : "";
    statusEl.textContent = `使用中: ${used} / ${HID_CONFIG.MACRO_SIZE} bytes${dirtyTag}`;
  }
  if (remainingEl) {
    remainingEl.textContent = `追加可能な残りバイト数: ${remaining}`;
  }
}

function renderMacroSlots() {
  const area = document.getElementById("macroSlots");
  if (!area) return;
  area.innerHTML = "";
  const selectableLimit = getMacroSelectableLimit(macros);

  for (let i = 0; i < HID_CONFIG.MACRO_COUNT; i++) {
    const macroLen = macros[i]?.length || 0;
    const b = document.createElement("button");
    b.textContent = (macroLen > 0) ? `m${i} (${macroLen})` : `m${i}`;

    const selectable = i <= selectableLimit;
    if (!selectable) {
      b.disabled = true;
      b.classList.add("disabled");
    }
    if (i === selectedMacroIndex) {
      b.classList.add("active");
    }

    b.onclick = () => {
      if (!selectable) return;
      selectedMacroIndex = i;
      renderMacroEditor();
    };
    area.appendChild(b);
  }
}

function renderMacroSequence() {
  const sequenceEl = document.getElementById("macroSequence");
  if (!sequenceEl) return;
  sequenceEl.innerHTML = "";

  const currentMacro = macros[selectedMacroIndex] || [];
  if (currentMacro.length === 0) {
    const empty = document.createElement("span");
    empty.className = "macro-sequence-empty";
    empty.textContent = "コード未設定";
    sequenceEl.appendChild(empty);
  } else {
    currentMacro.forEach((usage, idx) => {
      const token = document.createElement("span");
      token.className = "macro-token";

      const label = document.createElement("span");
      label.textContent = getMacroUsageLabel(usage);

      const removeBtn = document.createElement("button");
      removeBtn.className = "remove-token";
      removeBtn.textContent = "x";
      removeBtn.title = "このコードを削除";
      removeBtn.onclick = () => removeMacroCodeAt(selectedMacroIndex, idx);

      token.appendChild(label);
      token.appendChild(removeBtn);
      sequenceEl.appendChild(token);
    });
  }

  const deleteLastBtn = document.getElementById("macroDeleteLastBtn");
  const clearBtn = document.getElementById("macroClearBtn");
  if (deleteLastBtn) deleteLastBtn.disabled = currentMacro.length === 0;
  if (clearBtn) clearBtn.disabled = currentMacro.length === 0;
}

function renderMacroEditor() {
  setMacrosIfFits(cloneMacros(), { silent: true });
  renderMacroStorageInfo();
  renderMacroSlots();
  renderMacroSequence();
}

function renderNumpad() {
  const container = document.getElementById("numpad");
  if (!container) return;
  container.innerHTML = "";
  container.style.gridTemplateColumns = "repeat(2, minmax(0, 1fr))";

  PHYSICAL_LAYOUT.forEach(config => {
    const keyData = keymaps[viewMode][config.id];
    const hasPending = pendingChanges.has(`${viewMode}:${config.id}`);
    const modifierNames = getModifierNamesFromUsage(keyData.usage);
    const baseLabel = getBaseLabelFromUsageForFace(keyData.usage, keyData.label);
    const modifierLine = modifierNames.join(" ");
    const faceLabelHtml = modifierLine
      ? `${escapeHtml(modifierLine)}<br>${escapeHtml(baseLabel)}`
      : escapeHtml(baseLabel);

    const btn = document.createElement("button");
    btn.className = "key";
    if (config.row === 0) {
      btn.classList.add("first-row");
    }
    if (config.id >= 2) {
      btn.classList.add("after-sw1");
    }

    btn.innerHTML = `<span>${faceLabelHtml}</span><div class="switchNo">${config.id}${hasPending ? ' ⏳' : ''}</div>`;
    if (baseLabel.length >= 6 || modifierNames.length > 0) {
      btn.classList.add("long-label");
    }
    if (hasFullWidthCharacter(baseLabel)) {
      btn.classList.add("fullwidth-label");
    }
    if (modifierNames.length > 0) {
      btn.classList.add("modifier-combo");
    }
    if (hasPending) {
      btn.classList.add("pending");
    }
    if (config.id === FN_SWITCH_ID && fnToggleOn) {
      btn.classList.add("fn-toggle-active");
      btn.setAttribute("aria-pressed", "true");
    }
    btn.onclick = () => {
      if (config.id === FN_SWITCH_ID) {
        fnToggleOn = !fnToggleOn;
        syncViewModeWithFnToggle();
        renderNumpad();
        updateLog(`Fn ${fnToggleOn ? "ON" : "OFF"}: Layer ${viewMode} を表示`);
        return;
      }
      selectedEncoderDirection = null;
      selectedSwitchId = config.id;
      document.getElementById("keyDialog").classList.remove("hidden");
    };

    if (config.id === 0) {
      const stack = document.createElement("div");
      stack.className = "sw0-stack";
      stack.style.gridRow = `${config.row + 1}`;
      stack.style.gridColumn = `${config.col + 1}`;

      const ring = document.createElement("div");
      ring.className = "sw0-encoder-circle";

      const encoderRow = document.createElement("div");
      encoderRow.className = "encoder-row";
      const cwData = encoderKeymaps[viewMode].cw;
      const ccwData = encoderKeymaps[viewMode].ccw;
      const cwPending = pendingEncoderChanges.has(`${viewMode}:cw`);
      const ccwPending = pendingEncoderChanges.has(`${viewMode}:ccw`);

      const makeEncoderBtn = (direction, label, data, pending) => {
        const b = document.createElement("button");
        b.className = "encoder-key";
        if (pending) b.classList.add("pending");
        const base = getBaseLabelFromUsageForFace(data.usage, data.label);
        b.innerHTML = `<span>${label}</span><small>${escapeHtml(base)}${pending ? " ⏳" : ""}</small>`;
        b.onclick = () => {
          selectedSwitchId = null;
          selectedEncoderDirection = direction;
          document.getElementById("keyDialog").classList.remove("hidden");
        };
        return b;
      };

      encoderRow.appendChild(makeEncoderBtn("ccw", "CCW", ccwData, ccwPending));
      encoderRow.appendChild(makeEncoderBtn("cw", "CW", cwData, cwPending));

      stack.appendChild(ring);
      stack.appendChild(btn);
      stack.appendChild(encoderRow);
      container.appendChild(stack);
    } else {
      btn.style.gridRow = `${config.row + 1} / span ${config.rowSpan || 1}`;
      btn.style.gridColumn = `${config.col + 1} / span ${config.colSpan || 1}`;
      container.appendChild(btn);
    }
  });
}

/** 5. 初期化とイベント **/
document.addEventListener("DOMContentLoaded", () => {
  enforceFixedFnSwitchAllLayers();
  syncViewModeWithFnToggle();

  if (navigator.hid) {
    navigator.hid.addEventListener("disconnect", handleHidDisconnect);
  }
  setConnectedUiState(false);
  
  // A. ダイアログのキーボタン生成
  const applySelectedKey = async (usage, label) => {
    if (selectedEncoderDirection) {
      encoderKeymaps[viewMode][selectedEncoderDirection] = { usage, label };
      pendingEncoderChanges.add(`${viewMode}:${selectedEncoderDirection}`);
      renderNumpad();
      document.getElementById("keyDialog").classList.add("hidden");
      updateLog(`Pending write: Encoder ${selectedEncoderDirection.toUpperCase()} = ${label} (press "キーマップを書き込む" to apply)`);
      selectedEncoderDirection = null;
      return;
    }

    if (selectedSwitchId === FN_SWITCH_ID) {
      document.getElementById("keyDialog").classList.add("hidden");
      updateLog("SW7 は固定 Fn キーです。割り当ては変更できません。");
      return;
    }

    keymaps[viewMode][selectedSwitchId] = { usage, label };
    pendingChanges.add(`${viewMode}:${selectedSwitchId}`);
    renderNumpad();
    document.getElementById("keyDialog").classList.add("hidden");
    updateLog(`Pending write: Switch ${selectedSwitchId} = ${label} (press "キーマップを書き込む" to apply)`);
  };

  const populate = (id, defs, labelResolver = (d) => d.label) => {
    const area = document.getElementById(id);
    if (!area) return;
    area.innerHTML = "";
    defs.forEach(d => {
      const b = document.createElement("button");
      const displayLabel = labelResolver(d);
      b.textContent = displayLabel;
      if (displayLabel.length === 1) {
        b.classList.add("single-char");
      }
      b.onclick = async () => {
        const applyModifiers = !isMacroTriggerUsage(d.usage);
        const usage = applyModifiers ? (d.usage | getSelectedModifierBits()) : d.usage;
        const label = buildLabelWithModifiers(
          displayLabel,
          applyModifiers ? getSelectedModifierNames() : []
        );
        await applySelectedKey(usage, label);
      };
      area.appendChild(b);
    });
  };

  const renderModifierKeys = () => {
    const area = document.getElementById("modifierKeys");
    if (!area) return;
    area.innerHTML = "";

    modifierKeyDefs.forEach((def) => {
      const b = document.createElement("button");
      b.textContent = def.label;
      b.classList.add("modifier-toggle");
      const isActive = selectedModifiers[def.key];
      b.setAttribute("aria-pressed", isActive ? "true" : "false");
      if (isActive) {
        b.classList.add("active");
      }
      b.onclick = () => {
        selectedModifiers[def.key] = !selectedModifiers[def.key];
        renderModifierKeys();
        populate("charKeys", charKeyDefs, getCharKeyDisplayLabel);
      };
      area.appendChild(b);
    });
  };

  renderModifierKeys();
  populate("charKeys", charKeyDefs, getCharKeyDisplayLabel);
  populate("editKeys", editKeyDefs);
  populate("jpExtKeys", japaneseExtKeyDefs);
  populate("specialKeyCodes", specialKeyDefs);
  populate("mouseSimKeys", mouseSimKeyDefs);
  populate("functionKeysF1ToF12", functionKeyDefs.slice(0, 12));
  populate("functionKeysF13ToF24", functionKeyDefs.slice(12));
  populate("macroKeys", macroKeyDefs);
  populateMacroUsageButtons("macroBasicKeys", macroBasicUsageDefs);
  populateMacroUsageButtons("macroModifierKeys", macroModifierUsageDefs);
  populateMacroUsageButtons("macroJpKeys", macroJpKeyDefs);
  populateMacroUsageButtons("macroDelayKeys", macroDelayKeyDefs);
  renderMacroEditor();

  document.getElementById("openMacroEditorBtn").onclick = () => {
    renderMacroEditor();
    document.getElementById("macroEditorDialog").classList.remove("hidden");
  };
  document.getElementById("macroDeleteLastBtn").onclick = () => removeLastMacroCode();
  document.getElementById("macroClearBtn").onclick = () => clearSelectedMacro();

  // C. 接続（バージョンチェック付）
  document.getElementById("connectBtn").onclick = async () => {
    try {
      const devs = await navigator.hid.requestDevice({
        filters: [{ vendorId: HID_CONFIG.VENDOR_ID, usagePage: HID_CONFIG.USAGE_PAGE, usage: HID_CONFIG.USAGE_ID }]
      });
      if (devs.length === 0) return;
      const dev = devs[0];
      await dev.open();
      device = dev;
      device.oninputreport = handleInputReport;

      const { body } = await sendCommand(HID_COMMANDS.GET_VERSION);
      const version = body[0] | (body[1] << 8);
      if (version !== HID_CONFIG.VERSION_NUM) throw new Error(`Version mismatch (Got 0x${version.toString(16)})`);

      setConnectedUiState(true);
      updateLog(`Connected: (Ver 0x${version.toString(16)})`);
      pendingChanges.clear();
      pendingEncoderChanges.clear();
      try {
        await syncEncoderKeysFromDevice();
      } catch (e) {
        updateLog(`READ_RE_KEY on connect failed: ${e.message}`);
      }
      await document.getElementById("readKeymapBtn").onclick();
      await readMacros(true);
      renderMacroEditor();
    } catch (e) {
      updateLog(`Connection Failed: ${e.message}`);
      if (device) {
        try {
          await device.close();
        } catch (_closeError) {
          // ignore
        }
      }
      clearConnectionState("Disconnected.", { log: false });
    }
  };

  // D. キーマップ読み出し
  document.getElementById("readKeymapBtn").onclick = async () => {
    if (!device) return updateLog("Not connected.");
    const maxKeysPerRequest = 14; // 28バイト（data[4]以降）÷ 2バイト/キー

    try {
      updateLog("Reading keymap + RE keys...");
      const fnMismatchLayers = [];
      const tempKeymaps = Array.from({ length: HID_CONFIG.LAYER_COUNT }, () =>
        new Array(HID_CONFIG.KEYMAP_SIZE).fill(null).map(() => ({ usage: 0, label: "None" }))
      );

      for (let layer = 0; layer < HID_CONFIG.LAYER_COUNT; layer++) {
        for(let start = 0; start < HID_CONFIG.KEYMAP_SIZE; start += maxKeysPerRequest) {
          const count = Math.min(maxKeysPerRequest, HID_CONFIG.KEYMAP_SIZE - start);
          const payload = new Uint8Array([start, count, layer  ]);
          const { body } = await sendCommand(HID_COMMANDS.READ_KEYMAP, payload);
          const returnedCount = body[0];
          if (returnedCount !== count) {
            throw new Error(`Expected ${count} keys but got ${returnedCount}`);
          }
          for (let i = 0; i < returnedCount; i++) {
            const offset = 3 + i * 2;
            const lo = body[offset];
            const hi = body[offset + 1];
            const usage = lo | (hi << 8);
            tempKeymaps[layer][start + i] = { usage, label: "" };
          }
        }
      }

      // デバイスから読み取ったUsageに対応するラベルを、そのまま表示に使う。
      for (let layer = 0; layer < HID_CONFIG.LAYER_COUNT; layer++) {
        if (tempKeymaps[layer][FN_SWITCH_ID].usage !== FN_USAGE) {
          fnMismatchLayers.push(layer);
        }
        for (let i = 0; i < HID_CONFIG.KEYMAP_SIZE; i++) {
          const usage = tempKeymaps[layer][i].usage;
          const usageLo = usage & 0xff;
          const label = buildLabelWithModifiers(
            MASTER_USAGE_MAP[usageLo] || `0x${usageLo.toString(16).toUpperCase()}`,
            getModifierNamesFromUsage(usage)
          );
          keymaps[layer][i] = { usage, label };
        }
      }

      pendingChanges.clear();
      enforceFixedFnSwitchAllLayers();
      fnToggleOn = false;
      syncViewModeWithFnToggle();
      fnMismatchLayers.forEach((layer) => {
        pendingChanges.add(`${layer}:${FN_SWITCH_ID}`);
      });
      await syncEncoderKeysFromDevice();
      pendingEncoderChanges.clear();
      renderNumpad();
      if (fnMismatchLayers.length > 0) {
        updateLog("SW7 は固定 Fn(0xBC) に補正されました。キーマップ書き込みでデバイスへ反映できます。");
      }
      updateLog("Read complete (keymap + RE).");
    } catch (e) { updateLog(`Read Error: ${e.message}`); }
  }

  // F. キーマップ書き込み（保留中のすべての変更をWRITE_KEYMAPで一括書き込み）
  document.getElementById("writeKeymapBtn").onclick = async () => {
    if (!device) return updateLog("Not connected.");
    if (pendingChanges.size === 0 && pendingEncoderChanges.size === 0) return updateLog("No pending changes to write.");

    const maxKeysPerRequest = 14; // 28バイト（data[4]以降）÷ 2バイト/キー

    try {
      updateLog("Writing keymap...");
      enforceFixedFnSwitchAllLayers();
      
      for (let layer = 0; layer < HID_CONFIG.LAYER_COUNT; layer++) {
        // このレイヤーで変更があるスイッチを集める
        const switchesToWrite = [];
        for (let switchId = 0; switchId < HID_CONFIG.KEYMAP_SIZE; switchId++) {
          if (pendingChanges.has(`${layer}:${switchId}`)) {
            switchesToWrite.push(switchId);
          }
        }

        if (switchesToWrite.length === 0) continue;

        // switchesToWrite を連続範囲に分割して送信
        // NOTE: スイッチが連続していない場合も WRITE_KEYMAP で扱える（全スイッチ分のデータが必要）
        // より効率的にするため、全スイッチを 1 つのリクエストで送信する
        //（最大 14 キーなので、複数に分割）
        
        for (let start = 0; start < switchesToWrite.length; start += maxKeysPerRequest) {
          const endIdx = Math.min(start + maxKeysPerRequest, switchesToWrite.length);
          const batch = switchesToWrite.slice(start, endIdx);
          const firstSwitchId = batch[0];
          const count = batch.length;

          // ペイロード準備：data[1]:開始スイッチ番号, data[2]:スイッチ数, data[3]:レイヤー, data[4]以降:キーコード
          const payload = new Uint8Array(3 + count * 2);
          payload[0] = firstSwitchId; // 開始スイッチ番号
          payload[1] = count;         // スイッチ数
          payload[2] = layer;         // レイヤー

          for (let i = 0; i < count; i++) {
            const switchId = batch[i];
            const usage = keymaps[layer][switchId].usage;
            payload[3 + i * 2 + 0] = usage & 0xFF;         // lower byte
            payload[3 + i * 2 + 1] = (usage >> 8) & 0xFF;  // upper byte
          }

          const { body } = await sendCommand(HID_COMMANDS.WRITE_KEYMAP, payload);
          const writtenCount = body[0];
          if (writtenCount !== count) {
            throw new Error(`Expected ${count} keys written but got ${writtenCount}`);
          }

          // 書き込み完了したスイッチを pendingChanges から削除
          for (const switchId of batch) {
            pendingChanges.delete(`${layer}:${switchId}`);
          }
        }
      }

      for (let layer = 0; layer < HID_CONFIG.LAYER_COUNT; layer++) {
        const hasLayerEncoderPending = pendingEncoderChanges.has(`${layer}:cw`) || pendingEncoderChanges.has(`${layer}:ccw`);
        if (!hasLayerEncoderPending) continue;

        const cwUsage = encoderKeymaps[layer].cw.usage;
        const ccwUsage = encoderKeymaps[layer].ccw.usage;
        await writeEncoderKeysForLayer(layer, 1, cwUsage, ccwUsage);
        pendingEncoderChanges.delete(`${layer}:cw`);
        pendingEncoderChanges.delete(`${layer}:ccw`);
      }

      renderNumpad();
      updateLog("Write complete. All pending changes have been applied.");
    } catch (e) { updateLog(`Write Error: ${e.message}`); }
  };

  // F2. マクロ書き込みボタン処理
  document.getElementById("writeMacrosBtn").onclick = async () => {
    if (!device) return updateLog("Not connected.");
    try {
      await writeMacros();
      renderMacroEditor();
    } catch (e) {
      updateLog(`Macro write error: ${e.message}`);
    }
  };

  // F3. マクロ読み出しボタン処理
  document.getElementById("readMacrosBtn").onclick = async () => {
    if (!device) return updateLog("Not connected.");
    if (hasUnsavedMacroEdits && !confirm("未保存のマクロ編集を破棄して読み出しますか？")) {
      return;
    }
    await readMacros(true);
    renderMacroEditor();
  };

  // G. デフォルトにリセット（送信後にバージョンチェックを行う）
  document.getElementById("resetKeymapBtn").onclick = async () => {
    if (!device) return updateLog("Not connected.");
    if (!confirm("デバイスのキーマップをデフォルトに戻しますか？")) return;

    try {
      updateLog("Resetting device...");
      // RESET_KEYMAP (0x10) コマンド送信
      const { body } = await sendCommand(HID_COMMANDS.RESET_KEYMAP);
      
      // デバイスから返ってきたバージョンをチェック
      const version = body[0] | (body[1] << 8);
      if (version !== HID_CONFIG.VERSION_NUM) {
        throw new Error(`Reset Error: Version mismatch (Got 0x${version.toString(16)})`);
      }

      updateLog(`Reset success (Ver 0x${version.toString(16)} verified). Re-syncing...`);
      
      // デバイス側が初期化されたので、最新の状態を読み出す
      await document.getElementById("readKeymapBtn").onclick();
      await readMacros(true);
      renderMacroEditor();
    } catch (e) {
      updateLog(`Reset Failed: ${e.message}`);
    }
  };

  // H. ブートローダー開始
  document.getElementById("startBootloaderBtn").onclick = async () => {
    if (!device) return updateLog("Not connected.");
    if (!confirm("ブートローダーを開始しますか？")) return;

    try {
      updateLog("Starting bootloader...");
      await sendCommand(HID_COMMANDS.START_BOOT_LOADER);
      updateLog("Bootloader started. Device may disconnect.");
    } catch (e) {
      updateLog(`Bootloader Start Failed: ${e.message}`);
    }
  };

 // ログクリアボタンのイベント
  document.getElementById("clearLogBtn").onclick = () => {
    const logEl = document.getElementById("log");
    if (logEl) logEl.textContent = "";
  };

  // NVMを読み出すボタンのイベント
  document.getElementById("readNvmBtn").onclick = async () => {
    if (!device) return updateLog("Not connected.");

    try {
      updateLog("Reading NVM...");
      const nvmData = new Uint8Array(HID_CONFIG.NVM_SIZE);
      // レスポンス: data[0]=cmd, data[1]=count, data[2..30]=データ → 最大29バイト
      const chunkSize = HID_CONFIG.RAW_DATA_SIZE - 3; // 29 bytes per request

      for (let offset = 0; offset < HID_CONFIG.NVM_SIZE; offset += chunkSize) {
        const length = Math.min(chunkSize, HID_CONFIG.NVM_SIZE - offset);
        const payload = new Uint8Array([offset, length]);
        const { body } = await sendCommand(HID_COMMANDS.READ_NVM, payload);
        let readlength = body[0];
        nvmData.set(body.slice(1, readlength + 1), offset); // slice終端は排他なので+1
      }

      // Format the data: 8 bytes per line, 2-digit hex with space
      let hexString = "";
      for (let i = 0; i < nvmData.length; i++) {
        if (i > 0 && i % 8 === 0) hexString += "\n";
        hexString += nvmData[i].toString(16).padStart(2, '0') + " ";
      }
      updateLog(`NVM Data:\n${hexString.trim()}`);
    } catch (e) {
      updateLog(`NVM Read Failed: ${e.message}`);
    }
  };

  // macro_ptrsを読み出すボタンのイベント
  document.getElementById("readMacroPtrsBtn").onclick = async () => {
    if (!device) return updateLog("Not connected.");

    try {
      const requestedBytes = 10;
      const payload = new Uint8Array([requestedBytes]);
      const { body } = await sendCommand(HID_COMMANDS.READ_MACRO_PTRS, payload);

      const readLength = Math.min(body[0], requestedBytes);
      const ptrData = body.slice(1, 1 + readLength);
      const hexString = Array.from(ptrData)
        .map((b) => b.toString(16).toUpperCase().padStart(2, "0"))
        .join(" ");

      updateLog(`macro ptrs (${readLength} bytes): ${hexString}`);
    } catch (e) {
      updateLog(`Macro ptrs read failed: ${e.message}`);
    }
  };


  document.getElementById("closeDialog").onclick = () => document.getElementById("keyDialog").classList.add("hidden");
  document.getElementById("closeSpecialDialog").onclick = () => document.getElementById("specialKeyDialog").classList.add("hidden");
  document.getElementById("closeMacroEditorDialog").onclick = () => document.getElementById("macroEditorDialog").classList.add("hidden");

  globalThis.macros = macros;
  renderNumpad();
  renderMacroEditor();
});
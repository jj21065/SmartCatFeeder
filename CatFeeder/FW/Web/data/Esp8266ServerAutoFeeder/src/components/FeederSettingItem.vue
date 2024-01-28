<script setup lang="ts">
import { onMounted, ref, reactive, CSSProperties } from 'vue'
import { Plus, ForkSpoon, Delete } from '@element-plus/icons-vue'
import { Timer } from '@element-plus/icons-vue'

// import { ElScrollbar } from 'element-plus'
import axios from 'axios'

import WelcomeItem from './WelcomeItem.vue'
import DocumentationIcon from './icons/IconDocumentation.vue'
import ToolingIcon from './icons/IconTooling.vue'
import EcosystemIcon from './icons/IconEcosystem.vue'
import CommunityIcon from './icons/IconCommunity.vue'
import SupportIcon from './icons/IconSupport.vue'
import { Console } from 'console'

// import { table } from 'console'
// import { Header } from 'element-plus/es/components/table-v2/src/components'
//import { METHODS } from 'http'

interface User {
  time: string
  date: string
  name: string
  amount: string
  enable: boolean
}
const tableData = reactive<User[]>([])

const maxAmount = ref(5)
const minAmount = ref(1)
const feedAmount = ref(1)
const loginDialogFormVisible = ref(false)
const isLogIn = ref(false)
const loginAccount = ref('')
const loginPassword = ref('')

let istableLoading = ref(true)

interface Mark {
  style: CSSProperties
  label: string
}
type Marks = Record<number, Mark | string>
const marks = reactive<Marks>({
  1: '1',
  2: '2',
  3: '3',
  4: '4',
  5: '5'
})
const timerSetting_value = ref(new Date())
const dayInital = [
  'EveryDay',
  'Monday',
  'Tuesday',
  'Wednesday',
  'Thursday',
  'Friday',
  'Saturday',
  'Sunday'
]
const daySelectionValue = ref([0])
const dayOptions = ref(
  Array.from({ length: 8 }).map((_, idx) => ({
    value: idx,
    label: `${dayInital[idx % 10]}` //${idx}`,
  }))
)
// 使用onMounted来注册生命周期钩子函数
onMounted(() => {
  // 在组件挂载后执行初始化函数
  isLogIn.value = false
  loginDialogFormVisible.value = true
   getFeedSchedule()
})

const sendLogInInfo = () => {
  axios
    .post('/checkAuthentication', {
      payload: {
        account: loginAccount.value.toString(),
        password: loginPassword.value.toString()
      }
    })
    .then((response) => {
      console.log(response)
      if (response.data.result == 'pass') {
        loginDialogFormVisible.value = false
        isLogIn.value = true
        ElNotification.success({
          title: 'Success',
          message: 'Log In Seccess!',
          offset: 100
        })
      } else {
        isLogIn.value = false

        ElNotification({
        title: 'Log In Fail!',
          message: 'Log In Fail!',
          type: 'warning',
          offset: 100
        })
      }
    })
    .catch((response) => {
      isLogIn.value = false

      ElNotification({
        title: 'Log In Fail!',
        message: response.message,
        offset: 100,
        type: 'error'
      })
    })
}

const sendFeedInfo = () => {
  axios
    .post('/feed', {
      amount: feedAmount.value.toString()
    })
    .then((response) => {
      console.log(response)
      ElNotification.success({
        title: 'Success',
        message: 'Upload Data Seccess!',
        offset: 100
      })
    })
    .catch((response) => {
      ElNotification({
        title: 'Feed API Fail',
        message: response.message,
        offset: 100,
        type: 'error'
      })
    })
}

const getFeedSchedule = () => {
  istableLoading.value = true
  let response: { data: any }
  axios
    .post('/getfeedScheduleInfo')
    .then((response) => {
      console.log(response)
      var responseData = response.data
      console.log(responseData)
      tableData.length = 0
      let timeList = responseData.time
      console.log(timeList)

      let dataLength = timeList.length
      let dateList = responseData.date

      console.log(dateList)
      let amountList = responseData.amount
      console.log(responseData.amount)
      let nameList = responseData.name

      let enableList = responseData.enable

      for (let i = 0; i < dataLength; i++) {
        let bEnable = true
        if (enableList[i].toLowerCase() != 'true') {
          bEnable = false
        }
        tableData.push({
          time: `${timeList[i]}`, //,'2016-05-03',
          date: `${dateList[i]}`, //,'2016-05-03',
          name: `${nameList[i]}`,
          amount: `${amountList[i]}`,
          enable: bEnable
        })
      }
      istableLoading.value = false
      ElNotification.success({
        title: 'Success',
        message: 'Load Data Seccess!',
        offset: 100
      })
    })
    .catch((response) => {
      console.log(response)

      istableLoading.value = false
      ElNotification({
        title: 'Get Feed Schedule API Fail',
        message: response.message,
        offset: 100,
        type: 'error'
      })
    })
}

const updateFeedSchedule = () => {
  istableLoading.value = true
  let timeArr = []
  let dateArr = []
  let amountArr = []
  let nameArr = []
  let enableArr = []
  tableData.forEach((element) => {
    timeArr.push(element.time)
    dateArr.push(element.date)
    amountArr.push(element.amount)
    nameArr.push(element.name)
    if (element.enable) {
      enableArr.push('true')
    } else {
      enableArr.push('false')
    }
  })
  axios
    .post('/feedSchedule', {
      time: timeArr,
      date: dateArr,
      name: nameArr,
      amount: amountArr,
      enable: enableArr
    })
    .then((response) => {
      console.log(response)
      istableLoading.value = false
      ElNotification.success({
        title: 'Success',
        message: 'Upload Data Seccess!',
        offset: 100
      })
    })
    .catch((response) => {
      console.log(response)
      istableLoading.value = true
      ElNotification({
        title: 'Send Feed Schedule API Fail',
        message: response.message,
        offset: 100,
        type: 'error'
      })
    })
}

const updateDaySelectionEvent = () => {
  daySelectionValue.value.sort()
  if (daySelectionValue.value.includes(0)) {
    daySelectionValue.value.length = 0
    daySelectionValue.value.push(0)
  }
  console.log(daySelectionValue.value)
}

const addAlarm = () => {
  // 新增設定到清單
  let timeString = timerSetting_value.value.toTimeString()
  timeString = timeString.split(' ')[0]

  console.log(timeString)
  let dayString = ''

  // daySelectionValue.value.forEach((element) => {
  //   dayString = dayString + ',' + dayInital[element]
  // })

  dayString = daySelectionValue.value.map((x) => dayInital[x]).join(',')
  console.log(dayString)
  tableData.push({
    time: `${timeString}`, //,'2016-05-03',
    date: `${dayString}`, //,'2016-05-03',
    name: `Alarm${tableData.length}`,
    amount: `${feedAmount.value}`,
    enable: true
  })

  updateFeedSchedule()
}

const handleAlarmEdit = (index: number, row: User) => {
  console.log(index, row)
}

const handleTableEnableEvent = () => {
  updateFeedSchedule()
}
const handleAlarmDelete = (index: number, row: User) => {
  console.log(index, row)
  tableData.splice(index, 1)
  updateFeedSchedule()
}
</script>

<template>
  <el-dialog v-model="loginDialogFormVisible" title="Log In">
    <el-form :model="form">
      <el-form-item label="Account Name" :label-width="formLabelWidth">
        <el-input autocomplete="off" v-model="loginAccount" />
      </el-form-item>
      <el-form-item label="Password" :label-width="formLabelWidth">
        <el-input autocomplete="off" v-model="loginPassword" />
      </el-form-item>
    </el-form>
    <template #footer>
      <span class="dialog-footer">
        <el-button @click="loginDialogFormVisible = false">Cancel</el-button>
        <el-button type="primary" @click="sendLogInInfo"> Confirm </el-button>
      </span>
    </template>
  </el-dialog>

  <el-button text @click="loginDialogFormVisible = true">
    <label>LogIn</label>
  </el-button>

  <WelcomeItem>
    <template #icon>
      <DocumentationIcon />
    </template>
    <template #heading>Documentation</template>
    <span> </span>
  </WelcomeItem>

  <WelcomeItem>
    <template #icon>
      <ToolingIcon />
    </template>
    <template #heading>Control</template>
    <div>
      <el-popconfirm
        confirm-button-text="Yes"
        cancel-button-text="No"
        :icon="InfoFilled"
        width="250"
        icon-color="#626AEF"
        title="Are you sure to you want to feed hoohoo this amount?"
        @confirm="sendFeedInfo"
        @cancel="cancelEvent"
      >
        <template #reference>
          <!-- <el-button type="primary" class="FeedBtn" :icon="ForkSpoon" @click="sendFeedInfo" round
          >Feed</el-button -->
          <el-button type="primary" class="FeedBtn" :icon="ForkSpoon" round :disabled="!isLogIn">Feed</el-button>
        </template>
      </el-popconfirm>
      <span> </span>
      <div class="slider-demo-block">
        <el-slider
          v-model="feedAmount"
          :max="maxAmount"
          :min="minAmount"
          show-input
          :marks="marks"
          dark
        />
      </div>
    </div>
  </WelcomeItem>

  <WelcomeItem>
    <template #icon>
      <EcosystemIcon />
    </template>
    <template #heading>Timer Setting</template>

    <div>
      <!-- <h1>鬧鐘時刻設定</h1> -->
      <!-- 手動設定時間 -->
      <div>
        <label for="alarmTime">設定時間：</label>
        <el-time-picker v-model="timerSetting_value" placeholder="Arbitrary time" />
      </div>

      <!-- 是否可周一至周五重複的設定 -->
      <div>
        <label for="repeat">重複：</label>

        <el-select
          v-model="daySelectionValue"
          placeholder="Please select"
          style="margin: 16px"
          multiple
        >
          <el-option
            v-for="item in dayOptions"
            :key="item.value"
            :label="item.label"
            :value="item.value"
            @click.native="updateDaySelectionEvent"
          >
          </el-option>
        </el-select>
        <el-button type="success" class="addAlarmBtn" @click="addAlarm" :icon="Plus" circle :disabled="!isLogIn"/>
      </div>

      <!-- 顯示設定清單 -->
      <div>
        <!-- <el-table v-loading="istableLoading" :data="tableData" style="width: 100%"> -->
        <el-table v-loading="istableLoading" :data="tableData" fit="true" size="small">
          <!-- <el-table  :data="tableData" style="width: 100%"> -->
          <el-table-column label="Time" width="120">
            <template #default="scope">
              <div style="display: flex; align-items: center">
                <el-icon><timer /></el-icon>
                <span style="margin-left: 10px">{{ scope.row.time }}</span>
              </div>
            </template>
          </el-table-column>
          <el-table-column label="Repeat" width="200">
            <template #default="scope">
              <div style="display: flex; align-items: center">
                <span style="margin-left: 10px">{{ scope.row.date }}</span>
              </div>
            </template>
          </el-table-column>
          <el-table-column label="Amount" width="70">
            <template #default="scope">
              <div style="display: flex; align-items: center">
                <span style="margin-left: 10px">{{ scope.row.amount }}</span>
              </div>
            </template>
          </el-table-column>
          <el-table-column label="Name" width="100">
            <template #default="scope">
              <el-popover effect="light" trigger="hover" placement="top" width="auto">
                <template #default>
                  <div>name: {{ scope.row.name }}</div>
                </template>
                <template #reference>
                  <!-- <el-tag>{{ scope.row.name }}</el-tag> -->
                  <el-input v-model="scope.row.name" placeholder="name" />
                </template>
              </el-popover>
            </template>
          </el-table-column>
          <el-table-column label="Operations">
            <template #default="scope">
              <!-- <el-button size="small" @click="handleAlarmEdit(scope.$index, scope.row)"
                >Edit</el-button
              > -->
              <el-switch
                v-model="tableData[scope.$index].enable"
                @change="handleTableEnableEvent()"
                :disabled="!isLogIn"
              />

              <el-button
                size="small"
                type="danger"
                @click="handleAlarmDelete(scope.$index, scope.row)"
                :icon="Delete"
                circle
                :disabled="!isLogIn"
              />
              <!-- 
              <el-button
                size="small"
                type="danger"
                @click="handleAlarmDelete(scope.$index, scope.row)"
                >Delete</el-button
              > -->
            </template>
          </el-table-column>
        </el-table>
      </div>
    </div>
  </WelcomeItem>

  <WelcomeItem>
    <template #icon>
      <CommunityIcon />
    </template>
    <template #heading>Community</template>

    Got stuck? Ask your question on
    <a href="https://chat.vuejs.org" target="_blank" rel="noopener">Vue Land</a>, our official
    Discord server, or
    <a href="https://stackoverflow.com/questions/tagged/vue.js" target="_blank" rel="noopener"
      >StackOverflow</a
    >. You should also subscribe to
    <a href="https://news.vuejs.org" target="_blank" rel="noopener">our mailing list</a> and follow
    the official
    <a href="https://twitter.com/vuejs" target="_blank" rel="noopener">@vuejs</a>
    twitter account for latest news in the Vue world.
  </WelcomeItem>

  <WelcomeItem>
    <template #icon>
      <SupportIcon />
    </template>
    <template #heading>Support Vue</template>

    As an independent project, Vue relies on community backing for its sustainability. You can help
    us by
    <a href="https://vuejs.org/sponsor/" target="_blank" rel="noopener">becoming a sponsor</a>.
  </WelcomeItem>
</template>

<style scoped>
.buttonText {
  color: var(--color-text);
}

.slider-demo-block {
  display: flex;
  align-items: center;
}
.slider-demo-block .el-slider {
  margin-top: 0;
  margin-left: 12px;
}
</style>
